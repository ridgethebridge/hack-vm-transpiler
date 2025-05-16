#include"vm_writer.h"
#include<string.h>
#include<dirent.h>
#include<stdlib.h>
#include<stdbool.h>
#define INPUT_ERROR 1
#define ARITHMETIC_ERROR 2
#define INVALID_INDEX 3
#define INVALID_SEGMENT 4
#define OVERFLOW_ERROR 5
typedef enum Flag {
	OUTPUT_FLAG = 0,
	NOT_FLAG
}Flag;

Flag flag_type(char *input)
{
	if(strcmp(input,"-o") == 0)
		return OUTPUT_FLAG;
	return NOT_FLAG;
}

void usage()
{
printf("VMtranslator - transpiles hack VM code into hack assembly code\n");
printf("usage: ./VMtranslator [flags] file...\n");
printf("flags:\n");
printf("-o <name>	names the assembly output to name specified\n");
}

int shift(int *argc, char ***argv)
{
	*argc-=1;
	*argv+=1;
}
int vm_translate(VM_Writer *writer, VM_Parser *parser)
{
	writer->cur_input_file = parser->file_name;
	String_Snap cur_function;
	while(vm_has_next(parser))
	{
		vm_skip_blanks(parser);
		if(!vm_has_next(parser))
			break;

		vm_read_line(parser);
		VM_Instruction ins = vm_read_instruction(parser);
		switch(ins)
		{
			case VM_PUSH: 
				{
				 VM_Segment segment = vm_read_segment(parser);
				uint16 index = vm_read_index(parser);
				if(ss_has_next(parser->line_scanner))
				{
					fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
					fprintf(stderr," excess arguments %.*s passed to binary operator push",parser->line_scanner.snap.length-1,parser->line_scanner.snap.data);
					return OVERFLOW_ERROR;
				}
				vm_write_push(writer,segment,index);
				 } break;
			case VM_ADD:
			case VM_SUB:
			case VM_NEG:
			case VM_EQ:
			case VM_GT:
			case VM_LT:
			case VM_OR:
			case VM_NOT:
			case VM_AND:
				{
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"arithmetic commands are unary, %.*s \n",parser->line_scanner.snap.length,parser->line_scanner.snap.data);
				return ARITHMETIC_ERROR;
			}
			vm_write_arithmetic(writer,ins);
				}break;

			case VM_POP: 
				{
				 VM_Segment segment = vm_read_segment(parser);
				uint16 index = vm_read_index(parser);
				if(ss_has_next(parser->line_scanner))
				{
					fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
					fprintf(stderr," excess arguments %.*s passed to binary operator pop\n",parser->line_scanner.snap.length-1,parser->line_scanner.snap.data);
					return OVERFLOW_ERROR;
				}
				vm_write_pop(writer,segment,index);
				 } break;

			case VM_FUNCTION:
				{
					String_Snap function_name = vm_get_name(parser);
					uint16 num_locals = vm_read_index(parser);
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
						fprintf(stderr," excess arguments %.*s passed to binary operator function",parser->line_scanner.snap.length-1,parser->line_scanner.snap.data);
						return OVERFLOW_ERROR;
					}
					cur_function = function_name;
					vm_write_function(writer,function_name,num_locals);
				}break;
			case VM_CALL:
				{
					String_Snap function_name = vm_get_name(parser);
					uint16 num_args = vm_read_index(parser);
					
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
						fprintf(stderr," excess arguments %.*s passed to binary operator call",parser->line_scanner.snap.length-1,parser->line_scanner.snap.data);
						return OVERFLOW_ERROR;
					}
					vm_write_call(writer,function_name,num_args);
				}break;

			case VM_LABEL:
				{
					String_Snap label_name = vm_get_name(parser);
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"too much stuff for label!\n");
						return 1;
					}
					vm_write_label(writer,label_name,cur_function);
				}break;
		
			case VM_GOTO:
				{
					String_Snap label  = vm_get_name(parser);
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"too much stuff for goto!\n");
						return 1;
					}
					vm_write_goto(writer,label,cur_function);
				}break;
			case VM_IF:
				{
					String_Snap label  = vm_get_name(parser);
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"too much stuff for if!\n");
						return 1;
					}
					vm_write_if(writer,label,cur_function);
				}break;
			case VM_RETURN:
				{
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"too much stuff for return!\n");
						return 1;
					}
					vm_write_return(writer);
				}break;
		}
	}
	vm_free_parser(parser);
}

int main(int argc, char **argv)
{
	bool dir_given = false;
	bool file_given = false;
	char *output_name;
	char buf[512];
	if(argc < 2)
	{
		fprintf(stderr,"need to put vm source files or directory\n");
		usage();
		return INPUT_ERROR;
	}


	VM_Writer *writer = vm_create_writer();
	writer->file_name = "output.asm";
	VM_Parser *parser;
	while(argc > 1) 
	{
		shift(&argc,&argv);
		Flag flag = flag_type(argv[0]);
		if( flag == OUTPUT_FLAG)
		{
			writer->file_name = argv[0];
			continue;
		}
	DIR *dir = opendir(argv[0]);
	if(dir)
	{
		if(file_given)
		{
			fprintf(stderr,"can only supply a list of files, or a directory but not both\n");
			return 9;
		}
		if(dir_given)
		{
			fprintf(stderr,"directory was supplied more than once, it can only be given once\n");
			return 9;
		}
		dir_given = true;
		struct dirent *entry;
		while((entry = readdir(dir)))
		{
			if(strstr(entry->d_name,".vm"))
			{
				snprintf(buf,sizeof(buf),"%s/%s",argv[0],entry->d_name);
				parser = vm_create_parser(buf);
				if(!parser)
				{
					fprintf(stderr,"could not open file %s for reading\n",buf);
					return 1;
				}
			vm_translate(writer,parser);
			}
			else continue;
		}
	}
	else 
	{
		if(dir_given)
		{
			fprintf(stderr,"can only supply a list of files, or a directory but not both\n");
			return 9;
		}
		parser = vm_create_parser(argv[0]);
		if(!parser)
		{
			fprintf(stderr,"could not open file %s for reading\n",argv[0]);
			return 1;
		}
		vm_translate(writer,parser);
		file_given = true;
	}
	}
	vm_output_to_file(writer);
	vm_free_writer(writer);
	return 0;
}
