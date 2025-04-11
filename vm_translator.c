
#include"vm_parser.h"
#include"vm_writer.h"

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
		return 1;
	}
	VM_Parser *parser = vm_create_parser(argv[1]);
	VM_Writer *writer = vm_create_writer("output.asm");

	while(vm_has_next(parser))
	{
		vm_read_line(parser);
		char *command = vm_get_word(parser);
		VM_Instruction ins = vm_instruction_type(command);
		if(ins == VM_PUSH)
		{
			char *segment = vm_get_word(parser);
			char *index = vm_get_word(parser);
			vm_write_push(writer,segment,index);
		}
		else if(ins == VM_ARITHMETIC)
		{
			vm_write_arithmetic(writer,command);
		}
		else if(ins == VM_POP)
		{
			char *segment = vm_get_word(parser);
			char *index = vm_get_word(parser);
			vm_write_pop(writer,segment,index);
		}
	}
	vm_free_parser(parser);
	vm_free_writer(writer);
}
