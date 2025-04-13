
#include<stdio.h>
#include<stdlib.h>
#include"vm_parser.h"

VM_Instruction vm_instruction_type(String_Snap ins)
{
	if(ss_are_equal(ins,SS("push")))
		return VM_PUSH;
	if(ss_are_equal(ins,SS("pop")) )
		return VM_POP;
	if(ss_are_equal(ins,SS("add")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("sub")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("neg")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("eq")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("gt")))
		return VM_PUSH;
	if(ss_are_equal(ins,SS("lt")) )
		return VM_POP;
	if(ss_are_equal(ins,SS("and")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("or")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("not")) )
		return VM_ARITHMETIC;
	return VM_INVALID_INSTRUCTION;
}

VM_Parser* vm_create_parser(char *file_name)
{
	#define INIT_SIZE 1024
	FILE* file = fopen(file_name,"r");
	if(!file)
		return NULL;
	char line_buf[INIT_SIZE];
	uint64 pos = 0;
	uint64 size = INIT_SIZE;
	VM_Parser* parser = malloc(sizeof(VM_Parser));
	char *code = malloc(sizeof(char)*1024);
	parser->file_name = malloc(strlen(file_name)+1);
	strcpy(parser->file_name,file_name);
	uint64 line_length;
	while(fgets(line_buf,1023,file))
	{
	line_length = strlen(line_buf);
	if(pos+line_length >= size)
	{
		size*=2;
		code = realloc(code,size);
	}
	strcpy(code+pos,line_buf);
	pos+=line_length;
	}
	code = realloc(code,pos);
	parser->code = ss_from_cstr(code);
	parser->cursor = 0;
	parser->line_num = 0;
	return parser;
}

bool vm_has_next(VM_Parser * parser)
{
	return (parser->cursor < parser->code.length);
}


 String_Snap vm_get_word(VM_Parser *parser)
{
	return ss_next_word(&(parser->line_scanner));
}

void vm_skip_blanks(VM_Parser *parser)
{
	bool space_found = true;
	while(space_found)
	{
		switch(parser->code.data[parser->cursor])
		{
			case ' ' :
			case '\t':
			case '\0':
				parser->cursor+=1;
				break;
			case '\n':
				parser->cursor+=1;
				parser->line_num++;
				break;
			case '/':
				if(parser->code.data[parser->cursor+1] == '/')
				{
					while(parser->code.data[parser->cursor++] != '\n')
						;
					parser->line_num++;
				}
				else
					space_found = false;
				break;
			default:
				space_found = false;
				break;
		}
	}
}


String_Snap vm_read_line(VM_Parser *parser)
{
	vm_skip_blanks(parser);
	String_Snap line = ss_delim_cstr(parser->code.data+parser->cursor,'\n');
	parser->cursor+=line.length+1;
	line = ss_trim(line);
	parser->line_scanner = ss_create_scanner(line);
	return parser->line_scanner.snap;
}

VM_Segment vm_segment_type(String_Snap segment)
{
	if(ss_are_equal(segment,SS("constant")))
		return VM_CONSTANT;
	if(ss_are_equal(segment,SS("static")))
		return VM_STATIC;
	if(ss_are_equal(segment,SS("local")))
		return VM_LOCAL;
	return VM_INVALID_SEGMENT;
}

