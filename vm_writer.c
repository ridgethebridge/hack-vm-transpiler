
#include"vm_parser.h"
#include"vm_writer.h"
#include<string.h>
#include<stdlib.h>

VM_Writer *vm_create_writer(char *file)
{
	FILE *output = fopen(file,"w");
	if(!output)
		return NULL;
	VM_Writer *writer = malloc(sizeof(VM_Writer));
	writer->file_name = malloc(strlen(file)+1);
	strcpy(writer->file_name,file);
	writer->output=output;
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

void vm_write_push(VM_Writer *writer, char *segment, char* index)
{
	
	if(strcmp(segment,"constant") == 0) 
	{
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M+1\n");
	}
	else if(strcmp(segment,"static") == 0)
	{
		
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@16\n");
		fprintf(writer->output,"A=D+A\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M+1\n");
	}
	
	/*
	push constant 8
	@8
	D=A
	@SP
	M=D
	A=A+1

	push static 8
	@8
	D=A
	@16
	A=A+D
	D=M
	@SP
	M=D
	A=A+1
	*/
}

void vm_write_arithmetic(VM_Writer *writer,char *command)
{
	if(strcmp(command,"add") == 0)
	{
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M-1\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"A=A-1\n");
		fprintf(writer->output,"M=D+M\n");
	}
}
		

void vm_write_pop(VM_Writer *writer,char *segment, char *index)
{

}
