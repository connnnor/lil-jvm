#ifndef LIL_JVM_MEMORY_H
#define LIL_JVM_MEMORY_H

#include "common.h"
#include "class.h"

#define ALLOCATE(type, count) \
  (type *)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) \
  ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, old_count, new_count) \
  (type*)reallocate(pointer, sizeof(type) * (old_count), \
    sizeof(type) * (new_count))

#define FREE_ARRAY(type, pointer, old_count) \
  reallocate(pointer, sizeof(type) * (old_count), 0)

void *reallocate(void *ptr, size_t old_size, size_t new_size);

struct class_file_t;

// we need to keep track of where we are on stack
// since we push & pop.
// don't need to keep track of locals because those are indexed
typedef struct frame_t {
    class_file_t *class_file;
    value_t *locals;
    value_t *stack;
    value_t *stack_top;
    uint16_t max_stack;
    uint16_t max_locals;
    uint8_t *code;
    uint16_t pc;
} frame_t;

#endif //LIL_JVM_MEMORY_H
