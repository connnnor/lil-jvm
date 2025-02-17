#include "class.h"
#include "common.h"
#include "memory.h"
#include <stdio.h>
#include "debug.h"
#include "vm.h"

static uint8_t *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(EXIT_FILE_ERROR);
    }

    fseek(f, 0L, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    uint8_t *buf = malloc(size + 1);
    if (buf == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(EXIT_FILE_ERROR);
    }
    size_t bytes_read = fread(buf, sizeof(char), size, f);
    if (bytes_read < size) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(EXIT_FILE_ERROR);
    }
    buf[bytes_read] = '\0';

    fclose(f);
    return buf;
}

int main(int argc, char **argv) {
    if (argc == 2) {
        class_file_t *class_file = ALLOCATE(class_file_t, 1);
        uint8_t *bytes = read_file(argv[1]);
        read_class_file(bytes, class_file);
        FREE(uint8_t, bytes);
        dump_classfile(class_file);
        FREE(class_file_t, class_file);
    } else {
        fprintf(stderr, "Usage: liljvm [path]\n");
    }
}
