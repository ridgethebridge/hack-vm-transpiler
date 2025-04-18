
#ifndef _VM_WRITER_H_
#define _VM_WRITER_H_
#include<stdio.h>
#include"vm_parser.h"

typedef struct VM_Writer {
	char *file_name;
	FILE *output;
} VM_Writer;


void vm_write_push(VM_Writer *writer, VM_Segment segment, uint16 index);
void vm_write_pop(VM_Writer *writer, VM_Segment segment, uint16 index);
void vm_write_arithmetic(VM_Writer *writer, VM_Op command);
VM_Writer *vm_create_writer(char *file);
void vm_free_writer(VM_Writer *writer);
void vm_write_label(VM_Writer *writer, String_Snap label, String_Snap function);
void vm_write_function(VM_Writer * writer, String_Snap function);
void vm_write_goto(VM_Writer * writer, String_Snap label,String_Snap function);
void vm_write_if(VM_Writer * writer, String_Snap label,String_Snap function);
void vm_write_call(VM_Writer *writer, String_Snap function, uint16 num_args);
void vm_write_return(VM_Writer *writer);

#endif
