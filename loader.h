#ifndef LIL_JVM_LOADER_H
#define LIL_JVM_LOADER_H

#include "class.h"
#include "common.h"
#include "memory.h"
#include <string.h>

void read_class_file(char *bytes, class_file_t *class_file);

char * get_constant_utf8(class_file_t *cf, uint16_t index);
char * get_classname(class_file_t *cf, uint16_t index);

#endif //LIL_JVM_LOADER_H
