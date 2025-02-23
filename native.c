#include "native.h"
#include "class.h"
#include "common.h"

void runtime_error(const char * format, ...);

//value_t println_native_int(int arg_count, value_t *args) {
//
//    printf("%d\n");
//    return INT_VAL(10);
//}
//
value_t exit_native(frame_t *f, int arg_count, value_t *args) {
    (void) f;
    if (arg_count != 1) {
        runtime_error("exit expected 1 arg but got %d", arg_count);
    }
    int code = AS_INT(args[0]);
    exit(code);
}

value_t println_str_native(frame_t *f, int arg_count, value_t *args) {
    (void) arg_count;
    value_t value = args[0];
    if (IS_REFERENCE(value)) {
        char *str = get_constant_utf8(f->class_file, AS_REFERENCE(value)->info.string_info.string_index);
        printf("%s\n", str);
    }
    // don't actually do anything with this
    return BOOL_VAL(true);
}

value_t println_bool_native(frame_t *f, int arg_count, value_t *args) {
    (void) arg_count;
    (void) f;
    printf("%s\n", AS_BOOL(args[0]) ? "true" : "false");
    // don't actually do anything with this
    return BOOL_VAL(true);
}


value_t println_int_native(frame_t *f, int arg_count, value_t *args) {
    (void) arg_count;
    (void) f;
    printf("%d\n", AS_INT(args[0]));
    // don't actually do anything with this
    return BOOL_VAL(true);
}



typedef enum native_method_t {
    EXIT,
    PRINTLN_STR,
    PRINTLN_INT,
    PRINTLN_BOOL,
    NATIVE_METHOD_NONE
} native_method_t;

native_fn_info_t native_method_map[] = {
        [EXIT] = {
            .class     = "java/lang/System",
            .method    = "exit",
            .desc      = "(I)V",
            .native_fn = exit_native,
            .arity     = 1
        },
        [PRINTLN_STR] = {
                .class     = "java/io/PrintStream",
                .method    = "println",
                .desc      = "(Ljava/lang/String;)V",
                .native_fn = println_str_native,
                .arity     = 1
        },
        [PRINTLN_BOOL] = {
                .class     = "java/io/PrintStream",
                .method    = "println",
                .desc      = "(Z)V",
                .native_fn = println_bool_native,
                .arity     = 1
        },
        [PRINTLN_INT] = {
            .class     = "java/io/PrintStream",
            .method    = "println",
            .desc      = "(I)V",
            .native_fn = println_int_native,
            .arity     = 1
        }
};

 native_fn_info_t *get_native_fn_info(char *class, char *method, char *desc) {
    for (int i = 0; i < NATIVE_METHOD_NONE; i++) {
        if (strcmp(class, native_method_map[i].class) == 0 &&
            strcmp(method, native_method_map[i].method) == 0 &&
            strcmp(desc, native_method_map[i].desc) == 0) {
            return &native_method_map[i];
        }
    }
    return NULL;
}

typedef enum native_field_t {
    STDOUT,
    STDERR,
    NATIVE_FIELD_NONE
} native_field_t;

native_field_info_t native_field_map[] = {
        [STDOUT] = {
                .class     = "java/lang/System",
                .name      = "out",
                .desc      = "Ljava/io/PrintStream;",
                .value     = &stdout
        },
        [STDERR] = {
                .class     = "java/lang/System",
                .name      = "err",
                .desc      = "Ljava/io/PrintStream;",
                .value     = &stderr
        },
};

void *get_native_field(char *class, char *name, char *desc) {
    for (int i = 0; i < NATIVE_FIELD_NONE; i++) {
        if (strcmp(class, native_field_map[i].class) == 0 &&
            strcmp(name,  native_field_map[i].name)  == 0 &&
            strcmp(desc,  native_field_map[i].desc)  == 0) {
            return &native_field_map[i].value;
        }
    }
    return NULL;
 }
