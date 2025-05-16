
#include"vm_writer.h"
#include<stdlib.h>
#include"arithmetic.h"
#include"push.h"
#include"pop.h"
#include<stdarg.h>

// counter for labels, used for comparison commands
static uint64 cond_counter = 0;
static uint64 return_counter = 0;

// list of strings containing segment names, accessed with enum values
static char segment_list[][5]= {
	"LCL","ARG","THIS",
	"THAT","THIS","5","SP",
	"LCL","ARG"
	};
// list of oeprations, accessed with enums
static char op_list[][256] = {
	ADD_OP,SUB_OP,EQ_OP,GT_OP,LT_OP,AND_OP,
	OR_OP,NOT_OP,NEG_OP
};

void vm_write(VM_Writer *writer,const char *str,...)
{
	va_list args;
	va_start(args,str);
	char temp[4096];
	vsprintf(temp,str,args);
	if(writer->pos + strlen(temp) >= writer->buffer_size)
	{
		writer->buffer_size = writer->buffer_size*2 + strlen(temp);
		writer->output = realloc(writer->output,writer->buffer_size);
	}
	strcpy(writer->output+writer->pos,temp);
	writer->pos=strlen(writer->output);
	va_end(args);
}
VM_Writer *vm_create_writer()
{
	#define INITIAL_OUTPUT_SIZE 1024
	VM_Writer *writer = malloc(sizeof(VM_Writer));
	writer->cur_input_file = NULL;
	writer->file_name = NULL;
	writer->output = malloc(INITIAL_OUTPUT_SIZE);
	writer->pos = 0;
	writer->buffer_size = INITIAL_OUTPUT_SIZE;
	// sets the stack up
	#define SP_INIT "@256\n"\
			   "D=A\n"\
			   "@SP\n"\
			   "M=D\n"
	vm_write(writer,"// Bootstrap\n");
	vm_write(writer,SP_INIT);
	vm_write_call(writer,SS("Sys.init"),0);
	return writer;
}

void vm_free_writer(VM_Writer *writer)
{
	//free(writer->file_name);
	free(writer->output);
	free(writer);
}

void vm_write_push(VM_Writer *writer, VM_Segment segment, uint16 index)
{

	char *segment_base = segment_list[segment]; 
	switch(segment)
	{
		case VM_STATIC:
			vm_write(writer,PUSH_STATIC,writer->cur_input_file,index);
			break;
		case VM_CONSTANT:
			vm_write(writer,PUSH_CONSTANT,index,segment_base);
			break;
		case VM_THIS:
		case VM_THAT:
		case VM_ARG:
		case VM_LCL:
			vm_write(writer,PUSH_POINTER,index,segment_base);
			break;
		default:
			vm_write(writer,PUSH_REG,index,segment_base);
	}
}

void vm_write_arithmetic(VM_Writer *writer,VM_Instruction ins)
{
	char *op = op_list[ins];
	switch(ins)
	{
		case VM_LT:
		case VM_GT:
		case VM_EQ:
			vm_write(writer,op,cond_counter,cond_counter,cond_counter);
			++cond_counter;
			break;
		default:
			vm_write(writer,op);
	}

}

void vm_write_pop(VM_Writer *writer,VM_Segment segment, uint16 index)
{
	char *segment_base = segment_list[segment]; // gets segment string

	switch(segment)
	{
		case VM_CONSTANT:
			fprintf(stderr,"cant pop into constant memory segment\n");
			fprintf(stderr,"%d\n",segment);
			exit(1);
			return;
		case VM_STATIC:
			vm_write(writer,POP_STATIC,writer->cur_input_file,index);
			return;
		case VM_THIS:
		case VM_THAT:
		case VM_ARG:
		case VM_LCL:
			vm_write(writer,POP_POINTER,index,segment_base);
			break;
		default:
			vm_write(writer,POP_REG,index,segment_base);
			break;
	}
}

//works
void vm_write_label(VM_Writer *writer, String_Snap label, String_Snap function)
{
	vm_write(writer,"// label\n");
	vm_write(writer,"(%.*s$%.*s)\n",function.length,function.data,label.length,label.data);
}

// works
void vm_write_function(VM_Writer *writer, String_Snap function,uint16 num_locals)
{
	vm_write(writer,"// function definition\n");
	vm_write(writer,"(%.*s)\n",function.length,function.data);
	for(uint16 i = 0; i < num_locals; ++i)
	{
		vm_write_push(writer,VM_CONSTANT, 0);
	}
}

//works
void vm_write_goto(VM_Writer *writer,String_Snap label, String_Snap function)
{
	vm_write(writer,"// goto\n");
	vm_write(writer,"@%.*s$%.*s\n",function.length,function.data,label.length,label.data);
	vm_write(writer,"0;JMP\n");
}

//works
void vm_write_if(VM_Writer *writer,String_Snap label, String_Snap function)
{
	vm_write(writer,"// if statement\n");

	#define IF_OUTPUT "@SP\n"\
		"AM=M-1\n"\
		"D=M\n"\
		"@%.*s$%.*s\n"\
		"D;JNE\n"

	vm_write(writer,IF_OUTPUT,function.length,function.data,label.length,label.data);

}

// works
void vm_write_call(VM_Writer *writer, String_Snap function,uint16 num_args)
{
	vm_write(writer,"// function call\n");
	// pushes return address onto stack
	#define RETURN_PUSH "@return.address.%lu\n"\
                          "D=A\n"\
                          "@SP\n"\
                          "A=M\n"\
                          "M=D\n"\
                          "@SP\n"\
                          "M=M+1\n"
	// return push is correct
	vm_write(writer,RETURN_PUSH,return_counter);

	// saves registers states
	// these pushes work
	vm_write_push(writer,VM_LCL_NO_DEREF,0);
	vm_write_push(writer,VM_ARG_NO_DEREF,0);
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
	vm_write(writer,WRITE_CALL,num_args,function.length,function.data,return_counter);
	++return_counter;
}

// looks like it works
void vm_write_return(VM_Writer *writer)
{
	vm_write(writer,"// return statement\n");
	#define SAVE_LCL "@LCL\n"\
			   "D=M\n"\
			   "@15\n"\
			   "M=D\n"\
			   "@5\n"\
			   "D=A\n"\
			   "@LCL\n"\
			   "A=M-D\n"\
			   "D=M\n"\
			   "@14\n"\
			   "M=D\n" // saves return address to gen2

	vm_write(writer,SAVE_LCL);
	vm_write_pop(writer,VM_ARG,0); // pops return value to 1st arg pushed, new top of stack
	
	#define STACK_RESTORE "@ARG\n"\
				"D=M\n"\
				"@SP\n"\
				"M=D+1\n"
	vm_write(writer,STACK_RESTORE);

	#define RESTORE_STATE "@15\n"\
                             "AM=M-1\n"\
                             "D=M\n"\
                             "@THAT\n"\
                             "M=D\n"\
				"@15\n"\
                                "AM=M-1\n"\
                                "D=M\n"\
                                "@THIS\n"\
                                "M=D\n"\
				"@15\n"\
                                "AM=M-1\n"\
                                "D=M\n"\
                                "@ARG\n"\
                                "M=D\n"\
				"@15\n"\
                                "AM=M-1\n"\
                                "D=M\n"\
                                "@LCL\n"\
                                "M=D\n"\
                             "@14\n"\
                             "A=M\n"\
                             "0;JMP\n"

	vm_write(writer,RESTORE_STATE);
}

void vm_output_to_file(VM_Writer *writer)
{
	FILE* file = fopen(writer->file_name,"w");
	fprintf(file,writer->output);
	fclose(file);
}
