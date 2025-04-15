
#include"vm_writer.h"
#include<stdlib.h>

static char segment_list[][5]= {
	XSTR(STATIC_BASE),XSTR(LOCAL_BASE),XSTR(ARG_BASE),XSTR(THIS_BASE),
	XSTR(THAT_BASE),XSTR(THIS_BASE),XSTR(TEMP_BASE),XSTR(STACK_BASE)
	};
static char op_list[][6] = {
	"M=D+M","M=M-D","M","M","M","M=D&M",
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

void vm_write_arithmetic(VM_Writer *writer,VM_Op command)
{
	char *op = op_list[command];
	switch(command)
	{
		case VM_ADD:
		case VM_SUB:
		case VM_AND:
		case VM_OR:
		case VM_LT:
		case VM_GT:
		case VM_EQ:
			fprintf(writer->output,"@SP\n");
			fprintf(writer->output,"M=M-1\n");
			fprintf(writer->output,"A=M\n");
			fprintf(writer->output,"D=M\n");
			fprintf(writer->output,"A=A-1\n");
			break;
		case VM_NOT:
		case VM_NEG:
			fprintf(writer->output,"@SP\n");
			fprintf(writer->output,"M=M-1\n");
			fprintf(writer->output,"A=M\n");
			break;
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

