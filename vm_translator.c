#include"vm_writer.h"

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
int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr,"need to put vm source files\n");
		usage();
		return INPUT_ERROR;
	}
	VM_Parser *parser = vm_create_parser(argv[1]);
	VM_Writer *writer = vm_create_writer("output.asm");
	// to be used for labels
	String_Snap last_function;

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
				fprintf(stderr,"segment is missing!\n");
				return INVALID_SEGMENT;
			}
			if(!index_snap.data)
			{
				fprintf(stderr,"index is missing!\n");
				return INVALID_INDEX;
			}
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"too much stuff on line!\n");
				return OVERFLOW_ERROR;
			}
			VM_Segment segment = vm_segment_type(segment_snap);
			if(segment == VM_INVALID_SEGMENT)
			{
				fprintf(stderr,"invalid segment %.*s\n",segment_snap.length, segment_snap.data);
				return INVALID_SEGMENT;
			}
			if(!vm_valid_index(index_snap))
			{
				fprintf(stderr,"invalid index %.*s\n",index_snap.length,index_snap.data);
				return INVALID_INDEX;
			}
			vm_write_push(writer,segment,index_snap);
		}
		else if(ins == VM_ARITHMETIC)
		{
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"error with arithmetic command on line %lu in file %s\n",parser->line_num,parser->file_name);
				fprintf(stderr,"arithmetic commands are unary %.*s should not come after %.*s\n",parser->line_scanner.snap.length,parser->line_scanner.snap.data,command.length,command.data);
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
				fprintf(stderr,"segment is missing!\n");
				return INVALID_SEGMENT;
			}
			if(!index_snap.data)
			{
				fprintf(stderr,"index is missing!\n");
				return INVALID_INDEX;
			}

			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"too much stuff on line!\n");
				return OVERFLOW_ERROR;
			}
			VM_Segment segment = vm_segment_type(segment_snap);
			if(segment == VM_INVALID_SEGMENT)
			{
				fprintf(stderr,"invalid segment %.*s\n",segment_snap.length, segment_snap.data);
				return INVALID_SEGMENT;
			}
			if(!vm_valid_index(index_snap))
			{
				fprintf(stderr,"invalid index %.*s\n",index_snap.length,index_snap.data);
				return INVALID_INDEX;
			}
			vm_write_pop(writer,segment,index_snap);
		}

		else if(ins == VM_FUNCTION)
		{
			String_Snap function_name = vm_get_word(parser);
			if(ss_has_next(parser->line_scanner))
			{
				fprintf(stderr,"too much stuff for function!\n");
			}
			if(!function_name.data)
			{
				fprintf(stderr,"missing name for function!\n");
			}
			last_function = function_name;
			vm_write_function(writer,function_name);
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
			
	}
	vm_free_parser(parser);
	vm_free_writer(writer);
return 0;
}
