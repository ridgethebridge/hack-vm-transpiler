
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
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("lt")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("and")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("or")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("not")) )
		return VM_ARITHMETIC;
	if(ss_are_equal(ins,SS("label")))
		return VM_LABEL;
	if(ss_are_equal(ins,SS("function")))
		return VM_FUNCTION;
	if(ss_are_equal(ins,SS("goto")))
		return VM_GOTO;
	if(ss_are_equal(ins,SS("if-goto")))
		return VM_IF;
	if(ss_are_equal(ins,SS("return")))
		return VM_RETURN;
	if(ss_are_equal(ins,SS("call")))
		return VM_CALL;
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
	fclose(file);
	return parser;
}

bool vm_has_next(VM_Parser * parser)
{
	return (parser->cursor < parser->code.length);
}


// returns empty snap with null on eol
 String_Snap vm_get_word(VM_Parser *parser)
{
	if(!ss_has_next(parser->line_scanner))
	{
		return (String_Snap) {
					.data = 0,
					.length = 0
				};
	}
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
	line = ss_strdelim(line,SS("//"));
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
	if(ss_are_equal(segment,SS("argument")))
		return VM_ARGUMENT;
	if(ss_are_equal(segment,SS("pointer")))
		return VM_POINTER;
	if(ss_are_equal(segment,SS("this")))
		return VM_THIS;
	if(ss_are_equal(segment,SS("that")))
		return VM_THAT;
	if(ss_are_equal(segment,SS("temp")))
		return VM_TEMP;
	return VM_INVALID_SEGMENT;
}

uint16 vm_index_to_uint16(String_Snap index)
{
	uint64 i = 0;
	uint16 number = 0;
	while(i < index.length)
	{
		if(!ss_isdigit(index.data[i]))
		{
			fprintf(stderr,"invalid index, not a number\n");
			exit(1);
		}
		number= number*10 +(index.data[i]-'0');
		++i;
	}
	return number;
}

void vm_free_parser(VM_Parser *parser)
{
	free(parser->code.data);
	free(parser->file_name);
	free(parser);
}

VM_Op vm_op_type(String_Snap op)
{
	if(ss_are_equal(op,SS("add")))
		return VM_ADD;
	if(ss_are_equal(op,SS("sub")))
		return VM_SUB;
	if(ss_are_equal(op,SS("and")))
		return VM_AND;
	if(ss_are_equal(op,SS("or")))
		return VM_OR;
	if(ss_are_equal(op,SS("eq")))
		return VM_EQ;
	if(ss_are_equal(op,SS("lt")))
		return VM_LT;
	if(ss_are_equal(op,SS("gt")))
		return VM_GT;
	if(ss_are_equal(op,SS("not")))
		return VM_NOT;
	if(ss_are_equal(op,SS("neg")))
		return VM_NEG;
	return VM_INVALID_OP;
}

