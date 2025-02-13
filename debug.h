#ifndef LIL_JVM_DEBUG_H
#define LIL_JVM_DEBUG_H

#include "common.h"
#include "class.h"

void dump_classfile(class_file_t *class_file);
int disassemble_inst(uint8_t *code, uint32_t offset, int indent_level);

#endif //LIL_JVM_DEBUG_H