#ifndef LIL_JVM_NATIVE_H
#define LIL_JVM_NATIVE_H

#include "class.h"


typedef value_t (*native_fn_t)(int arg_count, value_t *args);

typedef struct native_t {
    const char *class;
    const char *method;
    const char *desc;
    native_fn_t native_fn;
} native_t;

native_fn_t *get_native_fn(char *class, char *method, char *desc);

#endif //LIL_JVM_NATIVE_H
