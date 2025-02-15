#include "vm.h"
#include <stdarg.h>
#include "common.h"
#include "memory.h"
#include "debug.h"
#include "class.h"

vm_t vm;

void runtime_error(const char * format, ...);

void runtime_error(const char * format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

//    for (int i = vm.frame_count - 1; i >= 0; i--) {
//        call_frame_t *frame = &vm.frames[i];
//        obj_function_t *function = frame->closure->function;
//        size_t instruction = frame->ip - frame->closure->function->chunk.code - 1;
//        fprintf(stderr, "[line %d] in ", function->chunk.lines[instruction]);
//        if (function->name == NULL) {
//            fprintf(stderr, "script\n");
//        } else {
//            fprintf(stderr, "%s()\n", function->name->chars);
//        }
//    }

    exit(-1);
}

// put value in frame's local variable table at index i
void frame_store_local(frame_t *f, value_t v, int i) {
    f->locals[i] = v;
}

value_t frame_get_local(frame_t *f, int i) {
    return f->locals[i];
}

void print_value(value_t value) {
    switch(value.type) {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_INT:    printf("%d",  AS_INT(value)); break;
        case VAL_LONG:   printf("%ld", AS_LONG(value)); break;
        case VAL_FLOAT:  printf("%f",  AS_FLOAT(value)); break;
        case VAL_DOUBLE: printf("%f",  AS_DOUBLE(value)); break;
        case VAL_ADDR:
        case VAL_REF:
            printf("TODO add VAL_ADDR & VAL_REF: print_value");
    }
}

void push(frame_t *f, value_t v) {
    *(f->stack_top) = v;
    f->stack_top++;
}

value_t pop(frame_t *f) {
    f->stack_top--;
    return *(f->stack_top);
}

//value_t pop_word(void) {
//    vm.stack_top--;
//    return *vm.stack_top;
//}

uint16_t get_stack_size(frame_t *f) {
    return f->stack_top - f->stack;
}

//static value_t peek(int distance) {
//    return vm.stack_top[-1 - distance];
//}

//frame_t *push_frame(frame_t *parent, uint8_t *code, class_file_t *class_file, uint16_t max_stack, uint16_t max_locals) {
frame_t *push_frame(uint8_t *code, class_file_t *class_file, uint16_t max_stack, uint16_t max_locals) {
    frame_t *frame = ALLOCATE(frame_t, 1);

    frame->class_file = class_file;
    frame->locals = ALLOCATE(value_t, max_locals);
    frame->max_locals = max_locals;
    frame->stack = ALLOCATE(value_t, max_stack);
    frame->max_stack = max_stack;
    frame->stack_top = frame->stack;
    frame->code = code;
    frame->pc = 0;

    vm.frames[vm.frame_count++] = frame;

    return frame;
}

int *pop_frame(frame_t *frame) {
    if (vm.frame_count <= 1) {
        runtime_error("popping last frame");
    }

    FREE(value_t, frame->locals);
    FREE(value_t, frame->stack);
    FREE(frame_t, frame);

    vm.frame_count--;

    return 0;
}

interpret_result_t run(void) {
    frame_t *frame = vm.frames[vm.frame_count - 1];
//    frame->stack = vm.stack;
//    frame->stack_top = vm.stack_top;
//#define READ_BYTE() (*frame[->ip++)
#define READ_BYTE() (frame->code[frame->pc++])

#define READ_SHORT() \
   (frame->pc += 2, \
   (uint16_t) ((frame->code[frame->pc - 2] << 8) | frame->code[frame->pc - 1]))

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("    Stack ");
    for (value_t *slot = frame->stack; slot < frame->stack_top; slot++) {
      printf("[ ");
      print_value(*slot);
      printf(" ]");
    }
    printf("\n");
    printf("   Locals ");
    for (uint16_t i = 0; i < frame->max_locals; i++) {
        printf("[ ");
        print_value(frame->locals[i]);
        printf(" ]");
    }
    printf("\n");
    disassemble_inst(frame->code, frame->pc, 0);
#endif
        frame = vm.frames[vm.frame_count - 1];
        uint8_t inst;
        switch (inst = READ_BYTE()) {
            case OP_ICONST1:
                push(frame, INT_VAL(1)); break;
            case OP_ICONST2:
                push(frame, INT_VAL(2)); break;
            case OP_ILOAD_0:
                push(frame, frame_get_local(frame, 0)); break;
            case OP_ILOAD_1:
                push(frame, frame_get_local(frame, 1)); break;
//            case OP_ALOAD_0:
//                return simple_inst("aload_0", offset);
//            case OP_INVOKE_SPECIAL:
//                return invoke_inst("invokespecial", code, offset);
            case OP_RETURN: {
                pop_frame(frame);
                break;
            }
            case OP_INVOKE_STATIC: {
                uint16_t index = READ_SHORT();

                constant_method_ref_info_t method_ref_info = get_constant(frame->class_file, index, CONSTANT_METHOD_REF)->info.method_ref_info;
                // lookup class & NameAndType in constant pool
                //constant_class_info_t class = get_constant(cf, method_ref_info.class_index, CONSTANT_CLASS);
                constant_name_and_type_info_t name_and_type = get_constant(
                        frame->class_file, method_ref_info.name_and_type_index, CONSTANT_NAME_AND_TYPE
                )->info.name_and_type_info;
                char *method_name = get_constant_utf8(frame->class_file, name_and_type.name_index);
                char *method_desc = get_constant_utf8(frame->class_file, name_and_type.descriptor_index);
                method_t *method = get_class_method(frame->class_file, method_name, method_desc);
                // create new frame
                attribute_t *attribute = get_attribute_by_tag(method->attribute_count, method->attributes, ATTR_CODE);
                attr_code_t *code_attr = AS_ATTR_CODE(attribute);
                frame_t *new_frame = push_frame(code_attr->code, frame->class_file, code_attr->max_stack, code_attr->max_locals);
                // pop nargs from current frames stack and push onto new frame's local variable table
                char *s = method_desc;
                int i = 0;
                if (*s != '(') {
                    runtime_error("expected description to begin with '(' but found %c", *s);
                }
                s++;
                while(s[i] != ')') {
                    frame_store_local(new_frame, pop(frame), i++);
                }
                break;
            }
//            case OP_ILOAD_0:
//                return simple_inst("iload_0", offset);
//            case OP_ILOAD_1:
//                return simple_inst("iload_1", offset);
            case OP_IADD: {
                int a = AS_INT(pop(frame));
                int b = AS_INT(pop(frame));
                push(frame, INT_VAL(a + b));
                break;
            }
//            case OP_IRETURN:
//                return simple_inst("ireturn", offset);
//            case OP_RETURN:
//                return simple_inst("return", offset);
            default:
                printf("Unknown opcode %d (0x%02x)\n", inst, inst);
                return INTERPRET_RUNTIME_ERROR;
        }
    }
}

static void reset_stack(void) {
    vm.frame_count = 0;
}

interpret_result_t interpret(class_file_t *class) {
    reset_stack();
    // find main method
    method_t *main = get_class_method(class, "main", "([Ljava/lang/String;)V");
    if (main == NULL) {
        runtime_error("error cannot find main method\n");
    }

    attribute_t *code_attr = get_attribute_by_tag(main->attribute_count, main->attributes, ATTR_CODE);
    push_frame(code_attr->info.attr_code->code, class, 2, 1);
    return run();
}
