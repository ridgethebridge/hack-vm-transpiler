
#ifndef _VM_PARER_H_
#define _VM_PARSER_H_
#include<stdbool.h>

// for freeing parser object
#define VM_FREE_PARSER(parser)\
	free(parser->code);\
	free(parser->file_name;\
	free(parser);
typedef unsigned long long uint64;
typedef unsigned short uint16;
typedef struct VM_Parser {
	 char *code;
	 char *file_name;
	 char *cur_line;
	uint64 length;
	uint64 cursor;
	uint64 line_cursor;
	uint64 line_num;
} VM_Parser;

typedef enum VM_Instruction {
	VM_PUSH,
	VM_POP,
	VM_ARITHMETIC,
	VM_INVALID_INSTRUCTION
} VM_Instruction;

typedef enum VM_Segment {
	VM_STATIC,
	VM_CONSTANT,
	VM_LOCAL,
	VM_INVALID_SEGMENT
}VM_Segment;
VM_Parser* vm_create_parser(char *file);

VM_Instruction vm_instruction_type(char *ins);

VM_Segment vm_segment_type(char *segment);

 char* vm_get_word(VM_Parser *parser);


bool vm_has_next();

void vm_skip_blanks(VM_Parser *parser);

char* vm_read_line(VM_Parser *parser);

void vm_free_parser(VM_Parser *parser);

#endif
