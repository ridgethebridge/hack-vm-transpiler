
#include<stdio.h>
#include<stdlib.h>
#include"vm_parser.h"
#include"../str-toolz/str_toolz.h"
//

VM_Instruction vm_instruction_type(char *ins)
{
	if(str_toolz_equal(ins,"push"))
		return VM_PUSH;
	if(str_toolz_equal(ins,"pop"))
		return VM_POP;
	if(str_toolz_equal(ins,"add"))
		return VM_ARITHMETIC;
	if(str_toolz_equal(ins,"sub"))
		return VM_ARITHMETIC;
	if(str_toolz_equal(ins,"neg"))
		return VM_ARITHMETIC;
	if(str_toolz_equal(ins,"eq") )
		return VM_ARITHMETIC;
	if(str_toolz_equal(ins,"gt") )
		return VM_ARITHMETIC;
	if(str_toolz_equal(ins,"lt") )
		return VM_ARITHMETIC;
	if(str_toolz_equal(ins,"and") )
		return VM_ARITHMETIC;
	if(str_toolz_equal(ins,"or") )
		return VM_ARITHMETIC;
	if(str_toolz_equal(ins,"not"))
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
	parser->code = malloc(sizeof(char)*1024);
	parser->file_name = malloc(strlen(file_name)+1);
	strcpy(parser->file_name,file_name);
	uint64 line_length;
	while(fgets(line_buf,1023,file))
	{
	line_length = strlen(line_buf);
	if(pos+line_length >= size)
	{
		size*=2;
		parser->code = realloc(parser->code,size);
	}
	strcpy(parser->code+pos,line_buf);
	pos+=line_length+1;
	}
	parser->code = realloc(parser->code,pos);
	parser->length = pos;
	parser->cursor = 0;
	parser->line_num = 0;
	parser->cur_line = NULL;
	parser->line_cursor = 0;
	return parser;
}

bool vm_has_next(VM_Parser * parser)
{
	return (parser->cursor < parser->length);
}


// had to check for null term
 char *vm_get_word(VM_Parser *parser)
{
	char *word = str_toolz_first_char(parser->cur_line + parser->line_cursor);
	parser->line_cursor = word-parser->cur_line;
	uint64 index = 0;
	while(!str_toolz_isspace(word[index]) && word[index] !='\0')
		++index;
	word[index] ='\0';
	parser->line_cursor+=index+1;
	return word;
}

void vm_skip_blanks(VM_Parser *parser)
{
	bool space_found = true;
	while(space_found)
	{
		switch(parser->code[parser->cursor])
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
				if(parser->code[parser->cursor+1] == '/')
				{
					while(parser->code[parser->cursor++] != '\n')
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

// just to see if its filled properly
void print_parser_contents(VM_Parser *parser)
{
	while(vm_has_next(parser))
	{
		parser->cursor+=strlen(parser->code+parser->cursor)+1;
	}
}

char *vm_read_line(VM_Parser *parser)
{
	if(parser->cur_line)
		free(parser->cur_line); // frees line as to avoid leaks
	vm_skip_blanks(parser);
	char *line_start = parser->code+parser->cursor; // line ends in newline
	char *line_end = str_toolz_eol(line_start);
	char *temp = str_toolz_substring(line_start,line_end);
	parser->cur_line = str_toolz_trim_comment(temp);
	parser->cursor+=strlen(temp)+2;
	parser->line_num+=1;
	parser->line_cursor=0;
	free(temp);
	return parser->cur_line;
}

VM_Segment vm_segment_type(char *segment)
{
	if(str_toolz_equal(segment,"constant"))
		return VM_CONSTANT;
	if(str_toolz_equal(segment,"static"))
		return VM_STATIC;
	if(str_toolz_equal(segment,"local"))
		return VM_LOCAL;
	return VM_INVALID_SEGMENT;
}

void vm_free_parser(VM_Parser *parser)
{
	free(parser->code);
	free(parser->file_name);
	free(parser->cur_line);
	free(parser);
}
