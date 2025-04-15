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

	while(vm_has_next(parser))
	{
		vm_read_line(parser);
	 	String_Snap command = vm_get_word(parser);
		VM_Instruction ins = vm_instruction_type(command);
		if(ins == VM_PUSH)
		{
			String_Snap segment_snap = vm_get_word(parser);
			String_Snap index_snap = vm_get_word(parser);
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
	}
	vm_free_parser(parser);
	vm_free_writer(writer);
return 0;
}
