#ifndef LIL_JVM_DEBUG_H
#define LIL_JVM_DEBUG_H

#include "common.h"
#include "class.h"

void dump_classfile(class_file_t *class_file);
int disassemble_inst(class_file_t *cf, uint8_t *code, uint32_t offset, int indent_level);
void print_constant_info(class_file_t *cf, constant_pool_t *constant);
void print_constant_info_at(class_file_t *cf, uint16_t index);
char *get_opcode_name(opcode_t opcode);

#endif //LIL_JVM_DEBUG_H
