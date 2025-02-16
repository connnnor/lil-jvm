#include "native.h"
#include "class.h"
#include "common.h"

//typedef value_t (*native_fn_t)(int arg_count, value_t *args);

void runtime_error(const char * format, ...);

//value_t println_native_int(int arg_count, value_t *args) {
//
//    printf("%d\n");
//    return INT_VAL(10);
//}
//
value_t exit_native(int arg_count, value_t *args) {
    if (arg_count != 1) {
        runtime_error("exit expected 1 arg but got %d", arg_count);
    }
    int code = AS_INT(args[0]);
    exit(code);
}

typedef enum native_method_t {
    EXIT,
    NATIVE_METHOD_NONE
} native_method_t;


// TODO - do this correctly ...
int num_natives = 0;

native_t native_map[] = {
        {
            .class     = "java/lang/System",
            .method    = "exit",
            .desc      = "(I)V",
            .native_fn = exit_native
        }
};


 native_fn_t *get_native_fn(char *class, char *method, char *desc) {
    for (int i = 0; i < NATIVE_METHOD_NONE; i++) {
        if (strcmp(class,  native_map[i].class)  == 0 &&
            strcmp(method, native_map[i].method) == 0 &&
            strcmp(desc,   native_map[i].desc)   == 0) {
            return &native_map[i].native_fn;
        }
    }
    return NULL;
}
