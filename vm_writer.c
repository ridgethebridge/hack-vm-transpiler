
#include"vm_writer.h"
#include<stdlib.h>

#define EQ_OP "D=M-D\n" \
	"@cond.%lu\n"\
	"D;JEQ\n"\
	"@SP\n"\
	"A=M-1\n"\
	"M=0\n"\
	"@3\n"\
	"D=A\n"\
	"@cond.%lu\n"\
	"A=D+A\n"\
	"0;JMP\n"\
	"(cond.%lu)\n"\
	"@SP\n"\
	"A=M-1\n"\
	"M=-1\n"

#define LT_OP "D=M-D\n" \
	"@cond.%lu\n"\
	"D;JLT\n"\
	"@SP\n"\
	"A=M-1\n"\
	"M=0\n"\
	"@3\n"\
	"D=A\n"\
	"@cond.%lu\n"\
	"A=D+A\n"\
	"0;JMP\n"\
	"(cond.%lu)\n"\
	"@SP\n"\
	"A=M-1\n"\
	"M=-1\n"

#define GT_OP "D=M-D\n" \
	"@cond.%lu\n"\
	"D;JGT\n"\
	"@SP\n"\
	"A=M-1\n"\
	"M=0\n"\
	"@3\n"\
	"D=A\n"\
	"@cond.%lu\n"\
	"A=D+A\n"\
	"0;JMP\n"\
	"(cond.%lu)\n"\
	"@SP\n"\
	"A=M-1\n"\
	"M=-1\n"
// counter for labels, used for comparison commands
static uint64 cond_counter = 0;
static uint64 return_counter = 0;

// list of strings containing segment names, accessed with enum values
static char segment_list[][5]= {
	XSTR(STATIC_BASE),XSTR(LOCAL_BASE),XSTR(ARG_BASE),XSTR(THIS_BASE),
	XSTR(THAT_BASE),XSTR(THIS_BASE),XSTR(TEMP_BASE),XSTR(STACK_BASE)
	};
// list of oeprations, accessed with enums
static char op_list[][256] = {
	"M=D+M","M=M-D",EQ_OP,GT_OP,LT_OP,"M=D&M",
	"M=D|M","M=!M","M=-M"
};

VM_Writer *vm_create_writer(char *file)
{
	FILE *output = fopen(file,"w");
	if(!output)
		return NULL;
	VM_Writer *writer = malloc(sizeof(VM_Writer));
	writer->file_name = malloc(strlen(file)+1);
	strcpy(writer->file_name,file);
	writer->output=output;
	// sets the stack up
	#define STACK_INIT "@256\n"\
			   "D=A\n"\
			   "@SP\n"\
			   "M=D\n"
//	fprintf(writer->output,STACK_INIT);
	return writer;
}

void vm_free_writer(VM_Writer *writer)
{
	free(writer->file_name);
	fclose(writer->output);
	free(writer);
}

void vm_write_push(VM_Writer *writer, VM_Segment segment, uint16 index)
{
		// always get constant index value
	char *segment_base = segment_list[segment]; 
	#define PUSH_INIT "@%hu\n"\
			  "D=A\n"\
			  "@%s\n"
	fprintf(writer->output,PUSH_INIT,index,segment_base);
	switch(segment)
	{
		case VM_CONSTANT:
			#define PUSH_CONSTANT "A=M\n"\
					      "M=D\n"\
					      "@SP\n"\
					      "M=M+1\n"
			fprintf(writer->output,PUSH_CONSTANT);
			return;
		case VM_THIS:
		case VM_THAT:
		case VM_ARGUMENT:
		case VM_LOCAL:
			fprintf(writer->output,"A=M\n");
			break;
	}
		// all  but constant have this config
		
	#define WRITE_PUSH "A=D+A\n"\
			   "D=M\n"\
			   "@SP\n"\
			   "A=M\n"\
                           "M=D\n"\
                           "@SP\n"\
                           "M=M+1\n"
	fprintf(writer->output,WRITE_PUSH);
}

// optimize this
void vm_write_arithmetic(VM_Writer *writer,VM_Op command)
{
	char *op = op_list[command];
	switch(command)
	{
		case VM_ADD:
		case VM_SUB:
		case VM_AND:
		case VM_OR:
			fprintf(writer->output,"@SP\n");
			fprintf(writer->output,"M=M-1\n");
			fprintf(writer->output,"A=M\n");
			fprintf(writer->output,"D=M\n");
			fprintf(writer->output,"A=A-1\n");
			break;
		case VM_NOT:
		case VM_NEG:
			fprintf(writer->output,"@SP\n");
			fprintf(writer->output,"A=M-1\n");
			break;
		case VM_LT:
			fprintf(writer->output,"@SP\n");
			fprintf(writer->output,"M=M-1\n");
			fprintf(writer->output,"A=M\n");
			fprintf(writer->output,"D=M\n");
			fprintf(writer->output,"A=A-1\n");
			fprintf(writer->output,LT_OP,cond_counter,cond_counter,cond_counter);
			++cond_counter;
			return;
		case VM_GT:
			fprintf(writer->output,"@SP\n");
			fprintf(writer->output,"M=M-1\n");
			fprintf(writer->output,"A=M\n");
			fprintf(writer->output,"D=M\n");
			fprintf(writer->output,"A=A-1\n");
			fprintf(writer->output,GT_OP,cond_counter,cond_counter,cond_counter);
			++cond_counter;
			return;
		case VM_EQ:
			fprintf(writer->output,"@SP\n");
			fprintf(writer->output,"M=M-1\n");
			fprintf(writer->output,"A=M\n");
			fprintf(writer->output,"D=M\n");
			fprintf(writer->output,"A=A-1\n");
			fprintf(writer->output,EQ_OP,cond_counter,cond_counter,cond_counter);
			++cond_counter;
			return;
		}
	// write the actual operation
	fprintf(writer->output,"%s\n",op);
}

void vm_write_pop(VM_Writer *writer,VM_Segment segment, uint16 index)
{
	fprintf(writer->output,"@%hu\n",index);
	fprintf(writer->output,"D=A\n");
	char *segment_base = segment_list[segment]; // gets segment string
	fprintf(writer->output,"@%s\n",segment_base);

	switch(segment)
	{
		case VM_THIS:
		case VM_THAT:
		case VM_ARGUMENT:
		case VM_LOCAL:
			fprintf(writer->output,"A=M\n");
			break;
	}
		// all do this routine

	#define WRITE_POP "A=D+A\n"\
                           "D=A\n"\
                           "@%s\n"\
                           "M=D\n"\
                           "@SP\n"\
                           "M=M-1\n"\
                           "A=M\n"\
                           "D=M\n"\
                           "@%s\n"\
                           "A=M\n"\
                           "M=D\n"
	fprintf(writer->output,WRITE_POP,XSTR(GEN_1),XSTR(GEN_1));

}

void vm_write_label(VM_Writer *writer, String_Snap label, String_Snap function)
{
	fprintf(writer->output,"(%.*s$%.*s)\n",function.length,function.data,label.length,label.data);
}

void vm_write_function(VM_Writer *writer, String_Snap function,uint16 num_locals)
{
	fprintf(writer->output,"(%.*s)\n",function.length,function.data);
	for(uint16 i = 0; i < num_locals; ++i)
	{
		vm_write_push(writer,VM_CONSTANT,0);
	}
}

void vm_write_goto(VM_Writer *writer,String_Snap label, String_Snap function)
{
	fprintf(writer->output,"@%.*s$%.*s\n",function.length,function.data,label.length,label.data);
	fprintf(writer->output,"0;JMP\n");
}

void vm_write_if(VM_Writer *writer,String_Snap label, String_Snap function)
{

	#define IF_OUTPUT "@SP\n"\
		"AM=M-1\n"\
		"D=M\n"\
		"@%.*s$%.*s\n"\
		"D;JNE\n"

	fprintf(writer->output,IF_OUTPUT,function.length,function.data,label.length,label.data);

}

void vm_write_call(VM_Writer *writer, String_Snap function,uint16 num_args)
{
	// pushes return address onto stack
	#define STACK_PUSH "@return.address.%lu\n"\
                          "D=A\n"\
                          "@SP\n"\
                          "A=M\n"\
                          "M=D\n"\
                          "@SP\n"\
                          "M=M+1\n"
	fprintf(writer->output,STACK_PUSH,return_counter);

	// saves registers states
	vm_write_push(writer,VM_LOCAL,0);
	vm_write_push(writer,VM_ARGUMENT,0);
	vm_write_push(writer,VM_POINTER,0); //pushing this
	vm_write_push(writer,VM_POINTER,1); // pushing that
	
	// sets arg, stack frame, and jumps to function
	#define WRITE_CALL "@5\n"\
                          "D=A\n"\
                          "@%hu\n"\
                          "D=D+A\n"\
                          "@SP\n"\
                          "D=M-D\n"\
                          "@ARG\n"\
                          "M=D\n"\
                          "@SP\n"\
                          "D=M\n"\
                          "@LCL\n"\
                          "M=D\n"\
			  "@%.*s\n"\
			  "0;JMP\n"\
			  "(return.address.%lu)\n"
	fprintf(writer->output,WRITE_CALL,num_args,function.length,function.data,return_counter);
	++return_counter;

}

void vm_write_return(VM_Writer *writer)
{
	#define SAVE_LCL "@LCL\n"\
			   "D=M\n"\
			   "@%s\n"\
			   "M=D\n"\
			   "@5\n"\
			   "D=A\n"\
			   "@LCL\n"\
			   "A=M\n"\
			   "A=A-D\n"\
			   "D=M\n"\
			   "@%s\n"\
			   "M=D\n" // saves return address to gen2

	fprintf(writer->output,SAVE_LCL,XSTR(GEN_3),XSTR(GEN_2));
	vm_write_pop(writer,VM_ARGUMENT,0); // pops return value to 1st arg pushed, new top of stack
	
	#define STACK_RESTORE "@ARG\n"\
				"D=M\n"\
				"@SP\n"\
				"M=D+1\n"
	fprintf(writer->output,STACK_RESTORE);

	#define RESTORE_STATE "@%s\n"\
                             "AM=M-1\n"\
                             "D=M\n"\
                             "@%s\n"\
                             "M=D\n"\
				"@%s\n"\
                                "AM=M-1\n"\
                                "D=M\n"\
                                "@%s\n"\
                                "M=D\n"\
				"@%s\n"\
                                "AM=M-1\n"\
                                "D=M\n"\
                                "@%s\n"\
                                "M=D\n"\
				"@%s\n"\
                                "AM=M-1\n"\
                                "D=M\n"\
                                "@%s\n"\
                                "M=D\n"\
                             "@%s\n"\
                             "A=M\n"\
                             "0;JMP\n"

	fprintf(writer->output,RESTORE_STATE,XSTR(GEN_3),XSTR(THAT_BASE),XSTR(GEN_3),XSTR(THIS_BASE),XSTR(GEN_3),XSTR(ARG_BASE),XSTR(GEN_3),XSTR(LOCAL_BASE),XSTR(GEN_2));
}
