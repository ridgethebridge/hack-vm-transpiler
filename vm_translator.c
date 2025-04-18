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
	 	String_Snap command = vm_get_word(parser);
		VM_Instruction ins = vm_instruction_type(command);
		if(ins == VM_PUSH)
		{
			String_Snap segment_snap = vm_get_word(parser);
			String_Snap index_snap = vm_get_word(parser);
			if(!segment_snap.data)
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"segment is missing in statement %.*s\n",segment_snap.length,segment_snap.data);
				return INVALID_SEGMENT;
			}
			if(!index_snap.data)
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"index is missing in statement %.*s\n",index_snap.length,index_snap.data);
				return INVALID_INDEX;
			}
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr," excess arguments %.*s passed to binary operator push",parser->line_scanner.snap.length-1,parser->line_scanner.snap.data);
				return OVERFLOW_ERROR;
			}
			VM_Segment segment = vm_segment_type(segment_snap);
			if(segment == VM_INVALID_SEGMENT)
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"segment %.*s is an invalid segment\n",segment_snap.length,segment_snap.data);
				return INVALID_SEGMENT;
			}
			uint16 index = vm_index_to_uint16(index_snap);
			vm_write_push(writer,segment,index);
		}
		else if(ins == VM_ARITHMETIC)
		{
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"arithmetic commands are unary, %.*s should not come after %.*s\n",parser->line_scanner.snap.length,parser->line_scanner.snap.data,command.length,command.data);
				return ARITHMETIC_ERROR;
			}
			VM_Op operation = vm_op_type(command);
			vm_write_arithmetic(writer,operation);
		}
		else if(ins == VM_POP)
		{
		 	String_Snap segment_snap = vm_get_word(parser);
			String_Snap index_snap  = vm_get_word(parser);
			if(!segment_snap.data)
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"segment is missing in statement %.*s\n",segment_snap.length,segment_snap.data);
				return INVALID_SEGMENT;
			}
			if(!index_snap.data)
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"index is missing in statement %.*s\n",index_snap.length,index_snap.data);
				return INVALID_INDEX;
			}

			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr," excess arguments %.*s passed to binary operator pop",parser->line_scanner.snap.length-1,parser->line_scanner.snap.data);
				return OVERFLOW_ERROR;
			}
			VM_Segment segment = vm_segment_type(segment_snap);
			if(segment == VM_INVALID_SEGMENT)
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"segment %.*s is an invalid segment\n",segment_snap.length,segment_snap.data);
				return INVALID_SEGMENT;
			}
			uint16 index = vm_index_to_uint16(index_snap);
			vm_write_pop(writer,segment,index);
		}

		else if(ins == VM_FUNCTION)
		{
			String_Snap function_name = vm_get_word(parser);
			String_Snap num_locals = vm_get_word(parser);
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr," excess arguments %.*s passed to binary operator function",parser->line_scanner.snap.length-1,parser->line_scanner.snap.data);
				return OVERFLOW_ERROR;
			}
			if(!function_name.data)
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"function name is missing\n");
				return 1;
			}
			last_function = function_name;
			vm_write_function(writer,function_name);
		}
		else if(ins == VM_CALL)
		{
			String_Snap function_name = vm_get_word(parser);
			String_Snap num_args = vm_get_word(parser);
			
			if(!function_name.data)
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"function name is missing for call operation\n");
				return -1;
			}
			if(!num_args.data)
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"number of arguments not specifed to call operator\n");
				return -1;
			}
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"error on line %lu : %s\n",parser->line_num,parser->file_name);
				fprintf(stderr," excess arguments %.*s passed to binary operator call",parser->line_scanner.snap.length-1,parser->line_scanner.snap.data);
				return OVERFLOW_ERROR;
			}
			uint16 args = vm_index_to_uint16(num_args);
			vm_write_call(writer,function_name,args);
		}

		else if(ins == VM_LABEL)
		{
			String_Snap label_name = vm_get_word(parser);
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"too much stuff for label!\n");
			}
			if(!label_name.data)
			{
				fprintf(stderr,"missing name for label!\n");
			}
			if(!last_function.data)
				last_function = SS("null");

			vm_write_label(writer,label_name,last_function);
		}
		
		else if(ins == VM_GOTO)
		{
			String_Snap label  = vm_get_word(parser);
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"too much stuff for goto!\n");
			}
			if(!label.data)
			{
				fprintf(stderr,"missing name for goto!\n");
			}

			vm_write_goto(writer,label,last_function);
		}
		else if(ins == VM_IF)
		{
			String_Snap label  = vm_get_word(parser);
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"too much stuff for if!\n");
			}
			if(!label.data)
			{
				fprintf(stderr,"missing name for if!\n");
			}

			vm_write_if(writer,label,last_function);
		}
		else if(ins == VM_RETURN)
		{
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"too much stuff for return!\n");
				return 90;
			}
			vm_write_return(writer);
			last_function = SS("null");

		}
			
	}
	vm_free_parser(parser);
	}
	vm_free_writer(writer);
return 0;
}
