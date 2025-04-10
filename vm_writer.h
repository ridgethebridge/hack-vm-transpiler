
#ifndef _VM_WRITER_H_
#define _VM_WRITER_H_
#include<stdio.h>

typedef struct VM_Writer {
	char *file_name;
	FILE *output;
} VM_Writer;


void vm_write_push(VM_Writer *writer, char *segment, char *index);
void vm_write_pop(VM_Writer *writer, char *segment, char *index);
void vm_write_arithmetic(VM_Writer *writer, char *command);
VM_Writer *vm_create_writer(char *file);
void vm_free_writer(VM_Writer *writer);

#endif
