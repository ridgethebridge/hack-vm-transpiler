
#ifndef _VM_PARER_H_
#define _VM_PARSER_H_
#include<stdbool.h>
#include"libs/string_snap.h"

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
	VM_ADD=0,
	VM_SUB,
	VM_EQ,
	VM_GT,
	VM_LT,
	VM_AND,
	VM_OR,
	VM_NOT,
	VM_NEG,
	VM_PUSH,
	VM_POP,
	VM_LABEL,
	VM_FUNCTION,
	VM_CALL,
	VM_IF,
	VM_GOTO,
	VM_RETURN,
	VM_INVALID_INSTRUCTION
} VM_Instruction;

typedef enum VM_Segment {
	VM_LCL=0,
	VM_ARG,
	VM_THIS,
	VM_THAT,
	VM_POINTER,
	VM_TEMP,
	VM_CONSTANT,
	VM_LCL_NO_DEREF,
	VM_ARG_NO_DEREF,
	VM_STATIC,
	VM_INVALID_SEGMENT
}VM_Segment;

VM_Parser* vm_create_parser(char *file);

VM_Instruction vm_instruction_type(String_Snap ins);
String_Snap vm_get_name(VM_Parser *parser);

VM_Segment vm_segment_type(String_Snap segment);

VM_Instruction vm_read_instruction(VM_Parser *parser);
VM_Segment vm_read_segment(VM_Parser *parser);
uint16 vm_read_index(VM_Parser *parser);


bool vm_has_next();

void vm_skip_blanks(VM_Parser *parser);

String_Snap vm_read_line(VM_Parser *parser);

void vm_free_parser(VM_Parser *parser);
uint16 vm_index_to_uint16(String_Snap index);
#endif
