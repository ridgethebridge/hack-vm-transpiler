
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
		return VM_ADD;
	if(ss_are_equal(ins,SS("sub")) )
		return VM_SUB;
	if(ss_are_equal(ins,SS("neg")) )
		return VM_NEG;
	if(ss_are_equal(ins,SS("eq")) )
		return VM_EQ;
	if(ss_are_equal(ins,SS("gt")))
		return VM_GT;
	if(ss_are_equal(ins,SS("lt")) )
		return VM_LT;
	if(ss_are_equal(ins,SS("and")) )
		return VM_AND;
	if(ss_are_equal(ins,SS("or")) )
		return VM_OR;
	if(ss_are_equal(ins,SS("not")) )
		return VM_NOT;
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
	code = realloc(code,pos+1);
	parser->code.data = code;
	parser->code.length = pos;
	parser->cursor = 0;
	parser->line_num = 0;
	parser->line_scanner.chars_read = 0;
	fclose(file);
	return parser;
}

bool vm_has_next(VM_Parser * parser)
{
	return (parser->cursor < parser->code.length);
}


// returns empty snap with null on eol
 VM_Instruction vm_read_instruction(VM_Parser *parser)
{
	if(!ss_has_next(parser->line_scanner))
	{
		fprintf(stderr,"error on line %lu:%lu in file %s\n",parser->line_num,parser->line_scanner.chars_read,parser->file_name);
		fprintf(stderr,"end of line reached when instruction expected\n");
		exit(1);
	}
	String_Snap ins =  ss_next_word(&(parser->line_scanner));
	VM_Instruction result = vm_instruction_type(ins);
	if(result == VM_INVALID_INSTRUCTION)
	{
		fprintf(stderr,"error on line %lu:%lu in file %s\n",parser->line_num,parser->line_scanner.chars_read,parser->file_name);
		fprintf(stderr,"in statement %.*s\n",parser->line_scanner.snap.data-parser->line_scanner.chars_read);
		fprintf(stderr,"invalid instruction %.*s\n",ins.length,ins.data);
		exit(1);
	}
	return result;
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
	if(!vm_has_next(parser))
	{
		fprintf(stderr,"error on line %lu:%lu in file %s\n",parser->line_num,parser->line_scanner.chars_read,parser->file_name);
		fprintf(stderr,"attempting to read line when the EOL has been reached\n");
		exit(1);
	}
	String_Snap line = ss_delim_cstr(parser->code.data+parser->cursor,'\n');
	parser->cursor+=line.length+1;
	line = ss_strdelim(line,SS("//"));
	line = ss_trim(line);
	parser->line_scanner = ss_create_scanner(line);
	parser->line_num+=1;
	return parser->line_scanner.snap;
}

VM_Segment vm_segment_type(String_Snap segment)
{
	if(ss_are_equal(segment,SS("constant")))
		return VM_CONSTANT;
	if(ss_are_equal(segment,SS("static")))
		return VM_STATIC;
	if(ss_are_equal(segment,SS("local")))
		return VM_LCL;
	if(ss_are_equal(segment,SS("argument")))
		return VM_ARG;
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


VM_Segment vm_read_segment(VM_Parser *parser)
{

	if(!ss_has_next(parser->line_scanner))
	{
		fprintf(stderr,"error on line %lu:%lu in file %s\n",parser->line_num,parser->line_scanner.chars_read,parser->file_name);
		fprintf(stderr,"in statement %.*s\n",parser->line_scanner.snap.data-parser->line_scanner.chars_read);
		fprintf(stderr,"end of line reached when segment expected\n");
		exit(1);
	}
	String_Snap segment = ss_next_word(&(parser->line_scanner));
	VM_Segment result = vm_segment_type(segment);

	if(result == VM_INVALID_SEGMENT)
	{
		fprintf(stderr,"error on line %lu:%lu in file %s\n",parser->line_num,parser->line_scanner.chars_read,parser->file_name);
		fprintf(stderr,"in statement %.*s\n",parser->line_scanner.snap.data-parser->line_scanner.chars_read);
		fprintf(stderr,"invalid segment %.*s\n",segment.length,segment.data);
		exit(1);
	}
	return result;
}

uint16 vm_read_index(VM_Parser *parser)
{
	if(!ss_has_next(parser->line_scanner))
	{
		fprintf(stderr,"error on line %lu:%lu in file %s\n",parser->line_num,parser->line_scanner.chars_read,parser->file_name);
		fprintf(stderr,"in statement %.*s\n",parser->line_scanner.snap.data-parser->line_scanner.chars_read);
		fprintf(stderr,"end of line reached when index expected\n");
		exit(1);
	}
	String_Snap index = ss_next_word(&(parser->line_scanner));
	VM_Segment result = vm_index_to_uint16(index);
	return result;
}

String_Snap vm_get_name(VM_Parser *parser)
{
	if(!ss_has_next(parser->line_scanner))
	{
		fprintf(stderr,"error on line %lu:%lu in file %s\n",parser->line_num,parser->line_scanner.chars_read,parser->file_name);
		fprintf(stderr,"in statement %.*s\n",parser->line_scanner.snap.data-parser->line_scanner.chars_read);
		fprintf(stderr,"end of line reached when function name expected\n");
		exit(1);
	}
	String_Snap name = ss_next_word(&(parser->line_scanner));
	if(name.data[0] >= '0' && name.data[0] <='9')
	{
		fprintf(stderr,"error on line %lu:%lu in file %s\n",parser->line_num,parser->line_scanner.chars_read,parser->file_name);
		fprintf(stderr,"in statement %.*s\n",parser->line_scanner.snap.data-parser->line_scanner.chars_read);
		fprintf(stderr,"invalid function name %.*s, has a digit at the start\n",name.length,name.data);
		exit(1);
	}
	return name;
}
