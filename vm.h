#ifndef LIL_JVM_VM_H
#define LIL_JVM_VM_H

#include "memory.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct {
    frame_t frames[FRAMES_MAX];
    int frame_count;

    value_t stack[STACK_MAX];
    value_t *stack_top;
} vm_t;

typedef enum interpret_result_t {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} interpret_result_t;

interpret_result_t interpret(class_file_t *class);
static interpret_result_t run();

#endif //LIL_JVM_VM_H
