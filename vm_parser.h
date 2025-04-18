
#ifndef _VM_PARER_H_
#define _VM_PARSER_H_
#include<stdbool.h>
#include"libs/string_snap.h"


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
	Snap_Scanner line_scanner;
	 char *file_name;
	uint64 cursor;
	uint64 line_num;
} VM_Parser;

typedef enum VM_Instruction {
	VM_PUSH = 0,
	VM_POP,
	VM_ARITHMETIC,
	VM_LABEL,
	VM_FUNCTION,
	VM_CALL,
	VM_IF,
	VM_GOTO,
	VM_RETURN,
	VM_INVALID_INSTRUCTION
} VM_Instruction;

typedef enum VM_Segment {
	VM_STATIC = 0,
	VM_LOCAL,
	VM_ARGUMENT,
	VM_THIS,
	VM_THAT,
	VM_POINTER,
	VM_TEMP,
	VM_CONSTANT,
	VM_INVALID_SEGMENT
}VM_Segment;

typedef enum VM_Op{
	VM_ADD= 0,
	VM_SUB,
	VM_EQ,
	VM_GT,
	VM_LT,
	VM_AND,
	VM_OR,
	VM_NOT,
	VM_NEG,
	VM_INVALID_OP
} VM_Op;

VM_Parser* vm_create_parser(char *file);

VM_Instruction vm_instruction_type(String_Snap ins);

VM_Segment vm_segment_type(String_Snap segment);

String_Snap vm_get_word(VM_Parser *parser);


bool vm_has_next();

void vm_skip_blanks(VM_Parser *parser);

String_Snap vm_read_line(VM_Parser *parser);

void vm_free_parser(VM_Parser *parser);
uint16 vm_index_to_uint16(String_Snap index);
void vm_free_parser(VM_Parser *parser);
VM_Op vm_op_type(String_Snap op);

#endif
