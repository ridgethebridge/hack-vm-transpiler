#include"vm_writer.h"
#include<string.h>

#define INPUT_ERROR 1
#define ARITHMETIC_ERROR 2
#define INVALID_INDEX 3
#define INVALID_SEGMENT 4
#define OVERFLOW_ERROR 5
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

int main(int argc, char **argv)
{
	char *output_name;
	if(argc < 2)
	{
		fprintf(stderr,"need to put vm source files\n");
		usage();
		return INPUT_ERROR;
	}


	VM_Writer *writer = vm_create_writer("output.asm");
	String_Snap last_function = {0,0};
	while(argc > 1) 
	{
		shift(&argc,&argv);
	VM_Parser *parser = vm_create_parser(argv[0]);
	// to be used for labels

	while(vm_has_next(parser))
	{
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
				}

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
					last_function = function_name;
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
				}

			case VM_LABEL:
				{
					String_Snap label_name = vm_get_name(parser);
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"too much stuff for label!\n");
						return 1;
					}
					if(!last_function.data)
						last_function = SS("null");

					vm_write_label(writer,label_name,last_function);
				}
		
			case VM_GOTO:
				{
					String_Snap label  = vm_get_name(parser);
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"too much stuff for goto!\n");
						return 1;
					}
					vm_write_goto(writer,label,last_function);
				}
			case VM_IF:
				{
					String_Snap label  = vm_get_name(parser);
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"too much stuff for if!\n");
						return 1;
					}
					vm_write_if(writer,label,last_function);
				}
			case VM_RETURN:
				{
					if(ss_has_next(parser->line_scanner))
					{
						fprintf(stderr,"too much stuff for return!\n");
						return 1;
					}
					vm_write_return(writer);
					last_function = SS("null");

				}
			
		}
	}
	vm_free_parser(parser);
	}
	vm_free_writer(writer);
	return 0;
}
