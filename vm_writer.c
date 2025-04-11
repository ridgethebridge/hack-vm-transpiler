
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
		fprintf(writer->output,"@SP\n"); fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M+1\n");
	}

	else if(strcmp(segment,"pointer"))
	{
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(THIS_BASE));
		fprintf(writer->output,"A=D+A\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M+1\n");
	}
	else if(strcmp(segment,"argument"))
	{
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@ARG\n");
		fprintf(writer->output,"A=D+A\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M+1\n");
	}
	else if(strcmp(segment,"local"))
	{
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@LCL\n");
		fprintf(writer->output,"A=D+A\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M+1\n");
	}
	else if(strcmp(segment,"this"))
	{
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(THIS_BASE));
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"A=D+A\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M+1\n");
	}
	else if(strcmp(segment,"that"))
	{
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(THAT_BASE));
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"A=D+A\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"M=D\n");
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M+1\n");
	}
	
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
	else if(strcmp(command,"sub") == 0)
	{
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M-1\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"A=A-1\n");
		fprintf(writer->output,"M=M-D\n");
	}
}

void vm_write_pop(VM_Writer *writer,char *segment, char *index)
{
	 if(strcmp(segment,"static") == 0)
	{
		
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(STATIC_BASE));
		fprintf(writer->output,"A=D+A\n");
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(GEN_1));
		fprintf(writer->output,"M=D\n"); //address of static region
		fprintf(writer->output,"@SP\n");
		fprintf(writer->output,"M=M-1\n");
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"D=M\n");
		fprintf(writer->output,"@%s\n",XSTR(GEN_1));
		fprintf(writer->output,"A=M\n"); // address of static region to pop to
		fprintf(writer->output,"M=D\n"); // address of static region to pop to
	}

	 else if(strcmp(segment,"local") == 0)
	{
		
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(LOCAL_BASE));
		fprintf(writer->output,"A=M\n");
		fprintf(writer->output,"A=D+A\n"); // add constant to local addr
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
	 else if(strcmp(segment,"argument") == 0)
	{
		
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(ARG_BASE));
		fprintf(writer->output,"A=M\n");
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
	 else if(strcmp(segment,"this") == 0)
	{
		
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(THIS_BASE));
		fprintf(writer->output,"A=M\n");
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
	 else if(strcmp(segment,"that") == 0)
	{
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(THAT_BASE));
		fprintf(writer->output,"A=M\n");
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
	 else if(strcmp(segment,"pointer") == 0)
	{
		
		fprintf(writer->output,"@%s\n",index);
		fprintf(writer->output,"D=A\n");
		fprintf(writer->output,"@%s\n",XSTR(THIS_BASE));
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

}
