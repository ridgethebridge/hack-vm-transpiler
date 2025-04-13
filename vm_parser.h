
#ifndef _VM_PARER_H_
#define _VM_PARSER_H_
#include<stdbool.h>
#include"../string-snap/string_snap.h"

// for freeing parser object
#define VM_FREE_PARSER(parser)\
	free(parser->file_name;\
	free(parser);

#define STATIC_BASE 16
#define STACK_BASE SP
#define LOCAL_BASE LCL
#define ARG_BASE ARG
#define THIS_BASE THIS
#define THAT_BASE THAT
#define TEMP_BASE 5
#define GEN_1 13
#define GEN_2 14
#define GEN_3 15
#define XSTR(X) STR(X)
#define STR(X) #X
typedef unsigned long long uint64;
typedef unsigned short uint16;
typedef struct VM_Parser {
	String_Snap code;
	String_Snap cur_line;
	 char *file_name;
	uint64 cursor;
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
	VM_LOCAL,
	VM_ARGUMENT,
	VM_THIS,
	VM_THAT,
	VM_POINTER,
	VM_TEMP,
	VM_CONSTANT,
	VM_INVALID_SEGMENT
}VM_Segment;
VM_Parser* vm_create_parser(char *file);

VM_Instruction vm_instruction_type(char *ins);

VM_Segment vm_segment_type(char *segment);

String_Snap vm_get_word(VM_Parser *parser);


bool vm_has_next();

void vm_skip_blanks(VM_Parser *parser);

String_Snap vm_read_line(VM_Parser *parser);

void vm_free_parser(VM_Parser *parser);

#endif
