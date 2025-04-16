
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
static uint64 cond_counter = 0;
static char segment_list[][5]= {
	XSTR(STATIC_BASE),XSTR(LOCAL_BASE),XSTR(ARG_BASE),XSTR(THIS_BASE),
	XSTR(THAT_BASE),XSTR(THIS_BASE),XSTR(TEMP_BASE),XSTR(STACK_BASE)
	};
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

void vm_write_push(VM_Writer *writer, VM_Segment segment, String_Snap index)
{
		// always get constant index value
		fprintf(writer->output,"@%.*s\n",index.length,index.data);
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

void vm_write_pop(VM_Writer *writer,VM_Segment segment, String_Snap index)
{
		fprintf(writer->output,"@%.*s\n",index.length,index.data);
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

