
#include<stdio.h>
#include<stdlib.h>
#include"vm_parser.h"
#include"../string-snap/string_snap.h"
//

VM_Instruction vm_instruction_type(char *ins)
{
	if(strcmp(ins,"push") == 0)
		return VM_PUSH;
	if(strcmp(ins,"pop") == 0)
		return VM_POP;
	if(strcmp(ins,"add") == 0)
		return VM_ARITHMETIC;
	if(strcmp(ins,"sub") == 0)
		return VM_ARITHMETIC;
	if(strcmp(ins,"neg") == 0)
		return VM_ARITHMETIC;
	if(strcmp(ins,"eq") ==0)
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
	pos+=line_length+1;
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
	return ss_next_word(&(parser->cur_line));
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
	parser->cur_line = ss_get_line(parser->code.data + parser->cursor);
	return parser->cur_line;
}

VM_Segment vm_segment_type(char *segment)
{
	if(strcmp(segment,"constant") == 0)
		return VM_CONSTANT;
	if(strcmp(segment,"static") == 0)
		return VM_STATIC;
	if(strcmp(segment,"local") == 0)
		return VM_LOCAL;
	return VM_INVALID_SEGMENT;
}

int main()
{
	VM_Parser *parser = vm_create_parser("paragraph.txt");
	vm_read_line(parser);
	ss_print_snap(parser->cur_line);
	return 0;
}
