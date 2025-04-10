
#include"vm_parser.h"
#include"vm_writer.h"
int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr,"need to put vm source file\n");
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
			printf("%s %s\n",segment,index);
			vm_write_push(writer,segment,index);
		}
		else if(ins == VM_ARITHMETIC)
		{
			vm_write_arithmetic(writer,command);
		}
	}
	vm_free_parser(parser);
	printf("sdsdsdsd");
	vm_free_writer(writer);
}
