#ifndef LIL_JVM_NATIVE_H
#define LIL_JVM_NATIVE_H

#include "class.h"
#include "memory.h"


typedef value_t (*native_fn_t)(frame_t *f, int arg_count, value_t *args);

typedef struct native_fn_info_t {
    const char *class;
    const char *method;
    const char *desc;
    native_fn_t native_fn;
    int arity;
} native_fn_info_t;

native_fn_info_t *get_native_fn_info(char *class, char *method, char *desc);

typedef struct native_field_info_t {
    const char *class;
    const char *name;
    const char *desc;
    void *value;
} native_field_info_t;

void *get_native_field(char *class, char *name, char *desc);

#endif //LIL_JVM_NATIVE_H
