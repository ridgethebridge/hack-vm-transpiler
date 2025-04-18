
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
	fprintf(output,"@256\n");
	fprintf(output,"D=A\n");
	fprintf(output,"@SP\n");
	fprintf(output,"M=D\n");
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
		fprintf(writer->output,"@%hu\n",index);
		fprintf(writer->output,"D=A\n");
		char *segment_base = segment_list[segment]; // gets segment string
		fprintf(writer->output,"@%s\n",segment_base);
	switch(segment)
	{
		case VM_CONSTANT:
			fprintf(writer->output,"A=M\n");
			fprintf(writer->output,"M=D\n");
			fprintf(writer->output,"@SP\n");
			fprintf(writer->output,"M=M+1\n");
			return;
		case VM_THIS:
		case VM_THAT:
		case VM_ARGUMENT:
		case VM_LOCAL:
			fprintf(writer->output,"A=M\n");
			break;
	}
		// all  but constant have this config
		fprintf(writer->output,"A=D+A\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"@SP\n"); 
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M+1\n");
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
		fprintf(writer->output,"A=D+A\n");
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(GEN_1));
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M-1\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"D=M\n"); 
		fprintf(writer->output,"@%s\n",XSTR(GEN_1)); 
		fprintf(writer->output,"A=M\n"); 
		fprintf(writer->output,"M=D\n"); 
}

void vm_write_label(VM_Writer *writer, String_Snap label, String_Snap function)
{
	fprintf(writer->output,"(%.*s$%.*s)\n",function.length,function.data,label.length,label.data);
}

void vm_write_function(VM_Writer *writer, String_Snap function)
{
	fprintf(writer->output,"(%.*s)\n",function.length,function.data);
}

void vm_write_goto(VM_Writer *writer,String_Snap label, String_Snap function)
{
	fprintf(writer->output,"@%.*s$%.*s\n",function.length,function.data,label.length,label.data);
	fprintf(writer->output,"0;JMP\n");
}

void vm_write_if(VM_Writer *writer,String_Snap label, String_Snap function)
{
	fprintf(writer->output,"@SP\n");
	fprintf(writer->output,"AM=M-1\n");
	fprintf(writer->output,"D=M\n");
	fprintf(writer->output,"@%.*s$%.*s\n",function.length,function.data,label.length,label.data);
	fprintf(writer->output,"D;JNE\n");
}

void vm_write_call(VM_Writer *writer, String_Snap function,uint16 num_args)
{
	// pushes return address onto stack
	fprintf(writer->output,"@return.address.%lu\n",return_counter);
	fprintf(writer->output,"D=A\n");
	fprintf(writer->output,"@SP\n");
	fprintf(writer->output,"A=M\n");
	fprintf(writer->output,"M=D\n");
	fprintf(writer->output,"@SP\n");
	fprintf(writer->output,"M=M+1\n");

	// saves registers states
	vm_write_push(writer,VM_LOCAL,0);
	vm_write_push(writer,VM_ARGUMENT,0);
	vm_write_push(writer,VM_POINTER,0); //pushing this
	vm_write_push(writer,VM_POINTER,1); // pushing that
	
	//changes arg to point to base of pushed arguments on stack
	fprintf(writer->output,"@5\n");
	fprintf(writer->output,"D=A\n");
	fprintf(writer->output,"@%hu\n",num_args);
	fprintf(writer->output,"D=D+A\n");
	fprintf(writer->output,"@SP\n");
	fprintf(writer->output,"D=M-D\n");
	fprintf(writer->output,"@ARG\n");
	fprintf(writer->output,"M=D\n");
	// sets locals base to top of the stack
	fprintf(writer->output,"@SP\n");
	fprintf(writer->output,"D=M\n");
	fprintf(writer->output,"@LCL\n");
	fprintf(writer->output,"M=D\n");
	// jumps to function and return address label
	fprintf(writer->output,"@%.*s\n",function.length,function.data);
	fprintf(writer->output,"0;JMP\n");
	fprintf(writer->output,"(return.address.%lu)\n",return_counter);
	++return_counter;

}

void vm_write_return(VM_Writer *writer)
{
	fprintf(writer->output,"@SP\n");
	fprintf(writer->output,"A=M\n");
	fprintf(writer->output,"D=M\n"); //return value
	fprintf(writer->output,"@%s\n",XSTR(GEN_1));
	fprintf(writer->output,"M=D\n");// holds return value
	fprintf(writer->output,"@LCL\n");
	fprintf(writer->output,"D=M\n");
	fprintf(writer->output,"@SP\n");
	fprintf(writer->output,"M=D\n"); // restores stack frame to beginning of frame
	vm_write_pop(writer,VM_POINTER,1);
	vm_write_pop(writer,VM_POINTER,0);
	vm_write_pop(writer,VM_ARGUMENT,0);
	vm_write_pop(writer,VM_LOCAL,0);
	fprintf(writer->output,"@SP\n");
	fprintf(writer->output,"A=M\n");
	fprintf(writer->output,"D=M\n");//return address
	fprintf(writer->output,"@%s\n",XSTR(GEN_2));
	fprintf(writer->output,"M=D\n");
	fprintf(writer->output,"@%s\n",XSTR(GEN_1));
	fprintf(writer->output,"D=M\n");
	fprintf(writer->output,"@SP\n");
	fprintf(writer->output,"A=M\n");
	fprintf(writer->output,"M=D\n"); //return value is at top of stack
}
