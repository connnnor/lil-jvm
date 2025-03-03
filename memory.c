#include "common.h"
#include "memory.h"

void *reallocate(void *ptr, size_t old_size, size_t new_size) {
    if (old_size == 0) {
        // don't complain about unused param old_size
        ;
    }
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }

    void *out = realloc(ptr, new_size);
    if (out == NULL) {
        exit(EXIT_MALLOC_ERROR);
    }
    return out;
}
