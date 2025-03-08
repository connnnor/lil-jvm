#include "vm.h"
#include <stdarg.h>
#include "common.h"
#include "memory.h"
#include "debug.h"
#include "class.h"
#include "native.h"

vm_t vm;

void runtime_error(const char * format, ...);

void runtime_error(const char * format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

//    for (int i = vm.frame_count - 1; i >= 0; i--) {
//        frame_t *frame = &vm.frames[i];
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

void call_native(frame_t *frame, native_fn_info_t *fn_info) {
//    if (native_fn_info->arity != arg_count) {
//        runtime_error("Expected %d arguments but got %d.",
//                      native_fn_info->arity, arg_count);
//    }
    native_fn_t native_fn = fn_info->native_fn;
    (*native_fn)(frame, fn_info->arity, frame->stack_top - fn_info->arity);
}

// put value in frame's local variable table at index i
void frame_store_local(frame_t *f, value_t v, int i) {
    f->locals[i] = v;
}

static char *value_type_t_str[] = {
        [VAL_BOOL] = "boolean",
        [VAL_INT] = "integer",
        [VAL_LONG] = "long",
        [VAL_FLOAT] = "float",
        [VAL_DOUBLE] = "double",
        [VAL_REF] = "reference",
        [VAL_ADDR] = "address",
};


// get local from frame and assert it is correct type
value_t frame_get_local_expect(frame_t *f, int i, value_type_t expected) {
    value_t v = f->locals[i];
    if (expected != v.type) {
        runtime_error("frame_get_local expected type %s but locals[%d] is type %s",
                      value_type_t_str[expected],
                      i,
                      value_type_t_str[v.type]);
    }
    return f->locals[i];
}

value_t frame_get_local(frame_t *f, int i) {
    return f->locals[i];
}

void print_value(frame_t *f, value_t value) {
    switch(value.type) {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_INT:    printf("%d",   AS_INT(value)); break;
        case VAL_LONG:   printf("%lld", (long long) AS_LONG(value)); break;
        case VAL_FLOAT:  printf("%f",   AS_FLOAT(value)); break;
        case VAL_DOUBLE: printf("%f",   AS_DOUBLE(value)); break;
        case VAL_ADDR: printf("TODO VAL_ADDR: print_value"); break;
        case VAL_REF:
            print_constant_info(f->class_file, value.as.reference);
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

value_t peek(frame_t *f, int distance) {
    return f->stack_top[-1 - distance];
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

// determine number of args from metod descriptor
// e.g. (II)V has 2 args
int get_nargs(char *desc) {
    char *s = desc;
    int i = 0;
    if (*s != '(') {
        runtime_error("expected description to begin with '(' but found %c", *s);
    }
    s++;
    while(s[i] != ')') {
        i++;
    }
    return i;
}

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
    FREE_ARRAY(value_t, frame->locals, frame->max_locals);
    FREE_ARRAY(value_t, frame->stack, frame->max_stack);
    FREE(frame_t, frame);

    vm.frame_count--;

    if (vm.frame_count == 0) {
        exit(0);
    }

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
#define COMPARE_BRANCH(a, b, op)                     \
  do {                                               \
    bool succeeds = a op b;                          \
    int16_t offset = READ_SHORT();                   \
    if (succeeds) {                                  \
        frame->pc += offset - 3;                     \
    }                                                \
  } while (false)

#define IS_VALUE_TYPE(value, value_type)  ((value).type == value_type)

// pop value from stack, verify value type and store as local
#define STORE_LOCAL(VAL_TYPE, index)                   \
    do {                                               \
    if (!IS_VALUE_TYPE(peek(frame, 0), VAL_TYPE)) {    \
      runtime_error("Operand must be type %s.",        \
        get_value_tag_name(VAL_TYPE));                 \
      return INTERPRET_RUNTIME_ERROR;                  \
    }                                                  \
    value_t v = pop(frame);                            \
    frame_store_local(frame, v, index);                \
  } while (false)

    for (;;) {
        frame = vm.frames[vm.frame_count - 1];
#ifdef DEBUG_TRACE_EXECUTION
        printf("\n");
        printf("    # Frames = %d", vm.frame_count);
        printf("    Stack ");
    for (value_t *slot = frame->stack; slot < frame->stack_top; slot++) {
      printf("[ ");
      print_value(frame, *slot);
      printf(" ]");
    }
    printf("\n");
    printf("   Locals ");
    for (uint16_t i = 0; i < frame->max_locals; i++) {
        printf("[ ");
        print_value(frame, frame->locals[i]);
        printf(" ]");
    }
    printf("\n");
    disassemble_inst(frame->class_file, frame->code, frame->pc, 0);
#endif
        uint8_t inst;
        switch (inst = READ_BYTE()) {
            case OP_ICONST_M1: // 0x02
                push(frame, INT_VAL(-1)); break;
            case OP_ICONST_0: // 0x03
                push(frame, INT_VAL(0)); break;
            case OP_ICONST_1: // 0x04
                push(frame, INT_VAL(1)); break;
            case OP_ICONST_2: // 0x05
                push(frame, INT_VAL(2)); break;
            case OP_BIPUSH: // 0x10
                push(frame, INT_VAL(READ_BYTE())); break;
            case OP_LDC: { // 0x12
                uint8_t index = READ_BYTE();
                constant_pool_t *constant = get_constant(frame->class_file, index);
                // if constant type
                switch(constant->tag) {
                    case CONSTANT_STRING: {
                        push(frame, REFERENCE_VAL(constant));
                        break;
                    }
                    default:
                        runtime_error("LDC unimplemented for constant type %s", get_constant_tag_name(constant->tag));
                }
                break;
            }
            case OP_LLOAD: // 0x16
                push(frame, frame_get_local_expect(frame, READ_BYTE(), VAL_LONG)); break;
            case OP_ILOAD_0: // 0x1a
                push(frame, frame_get_local(frame, 0)); break;
            case OP_ILOAD_1:
                push(frame, frame_get_local(frame, 1)); break;
//            case OP_ALOAD_0:
//                return simple_inst("aload_0", offset);
//            case OP_INVOKE_SPECIAL:
//                return invoke_inst("invokespecial", code, offset);
            case OP_ISTORE_0: // 0x3b,
                STORE_LOCAL(VAL_INT, 0); break;
            case OP_ISTORE_1: // 0x3c,
                STORE_LOCAL(VAL_INT, 1); break;
            case OP_ISTORE_2: // 0x3d,
                STORE_LOCAL(VAL_INT, 2); break;
            case OP_ISTORE_3: // 0x3e,
                STORE_LOCAL(VAL_INT, 3); break;
            case OP_IRETURN: { // 0xac
                // If no exception is thrown, value is popped from the operand stack of the current frame (§2.6)
                // and pushed onto the operand stack of the frame of the invoker
                frame_t *invoker_frame = vm.frames[vm.frame_count - 2];
                push(invoker_frame, pop(frame));
                pop_frame(frame);
                break;
            }
            case OP_RETURN: { // 0xb1
                pop_frame(frame);
                break;
            }
            case OP_GETSTATIC: { // 0xb2
                uint16_t index = READ_SHORT();
//                constant_pool_t *constant = get_constant_exp(frame->class_file, index, CONSTANT_FIELDREF);
                (void) index;
                // for now just do nothing
                break;
            }
            case OP_INVOKEVIRTUAL:  // 0xb6
            case OP_INVOKESTATIC: { // 0xb8
                uint16_t index = READ_SHORT();
                constant_method_ref_info_t method_ref_info = get_constant_exp(frame->class_file, index,
                                                                              CONSTANT_METHOD_REF)->info.method_ref_info;
                char *class_name = get_classname(frame->class_file, method_ref_info.class_index);
                // lookup class & NameAndType in constant pool
                //constant_class_info_t class = get_constant_exp(cf, method_ref_info.class_index, CONSTANT_CLASS);
                constant_name_and_type_info_t name_and_type = get_constant_exp(
                        frame->class_file, method_ref_info.name_and_type_index, CONSTANT_NAME_AND_TYPE
                )->info.name_and_type_info;
                char *method_name = get_constant_utf8(frame->class_file, name_and_type.name_index);
                char *method_desc = get_constant_utf8(frame->class_file, name_and_type.descriptor_index);
                method_t *method = get_class_method(frame->class_file, method_name, method_desc);
                if (method == NULL) {
                    native_fn_info_t *native_fn_info = get_native_fn_info(class_name, method_name, method_desc);
                    if (native_fn_info == NULL) {
                        runtime_error("cannot resolve method %s.%s:%s\n", class_name, method_name,
                                      method_desc);
                    }
                    call_native(frame, native_fn_info);
                    break;
                }
                // create new frame
                attribute_t *attribute = get_attribute_by_tag(method->attribute_count, method->attributes, ATTR_CODE);
                attr_code_t *code_attr = AS_ATTR_CODE(attribute);
                frame_t *new_frame = push_frame(code_attr->code, frame->class_file, code_attr->max_stack, code_attr->max_locals);
                // TODO - validate stack has nargs
                // TODO - validate stack value's type matches expected type from desc
                // pop nargs from current frames stack and push onto new frame's local variable table
                int nargs = get_nargs(method_desc);
                char *s = method_desc;
                int i = 0;
                if (*s != '(') {
                    runtime_error("expected description to begin with '(' but found %c", *s);
                }
                s++;
                while(s[i] != ')') {
                    // order should match b/w stack & locals, so stack = [0, 1, 2] -> someMethod(0, 1, 2)
                    frame_store_local(new_frame, pop(frame), nargs - i - 1);
                    i++;
                }
                break;
            }
            case OP_IADD: { // 0x60
                int a = AS_INT(pop(frame));
                int b = AS_INT(pop(frame));
                push(frame, INT_VAL(a + b));
                break;
            }
            case OP_INEG: { // 0x74
                int a = AS_INT(pop(frame));
                push(frame, INT_VAL(-a));
                break;
            }
            case OP_IINC: { // 0x84
                uint8_t index = READ_BYTE();
                int constant = READ_BYTE();
                value_t v = frame_get_local(frame, index);
                v.as.integer += constant;
                frame_store_local(frame, v, index);
                break;
            }
            case OP_IFEQ: // 0x99
                COMPARE_BRANCH(AS_INT(pop(frame)), 0, ==); break;
            case OP_IFNE: // 0x9a
                COMPARE_BRANCH(AS_INT(pop(frame)), 0, !=); break;
            case OP_IFLT: // 0x9b
                COMPARE_BRANCH(AS_INT(pop(frame)), 0, <); break;
            case OP_IFGE: // 0x9c
                COMPARE_BRANCH(AS_INT(pop(frame)), 0, >=); break;
            case OP_IFGT: // 0x9d
                COMPARE_BRANCH(AS_INT(pop(frame)), 0, >); break;
            case OP_IFLE: // 0x9e
                COMPARE_BRANCH(AS_INT(pop(frame)), 0, <=); break;
            case OP_IF_ICMPEQ: { // 0x9f
                COMPARE_BRANCH(AS_INT(pop(frame)), AS_INT(pop(frame)), ==);
                break;
            }
            case OP_IF_ICMPNE: { // 0xa0
                COMPARE_BRANCH(AS_INT(pop(frame)), AS_INT(pop(frame)), !=);
                break;
            }
            case OP_IF_ICMPLT: { // 0xa1
                value_t a = pop(frame);
                value_t b = pop(frame);
                COMPARE_BRANCH(AS_INT(b), AS_INT(a), <);
                break;
            }
            case OP_IF_ICMPGE: { // 0xa2
                value_t a = pop(frame);
                value_t b = pop(frame);
                COMPARE_BRANCH(AS_INT(b), AS_INT(a), >=);
                break;
            }
            case OP_IF_ICMPGT: { // 0xa3
                value_t a = pop(frame);
                value_t b = pop(frame);
                COMPARE_BRANCH(AS_INT(b), AS_INT(a), >);
                break;
            }
            case OP_IF_ICMPLE: { // 0xa4
                value_t a = pop(frame);
                value_t b = pop(frame);
                COMPARE_BRANCH(AS_INT(b), AS_INT(a), <=);
                break;
            }
            case OP_GOTO: // 0xa4
                COMPARE_BRANCH(true, true, ==); break; // always branch
            default:
                printf("Unimplemented opcode %s (0x%02x)\n", get_opcode_name(inst), inst);
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

    attr_code_t *code_attr = get_attribute_by_tag(main->attribute_count, main->attributes, ATTR_CODE)->info.attr_code;
    push_frame(code_attr->code, class, code_attr->max_stack, code_attr->max_locals);
    return run();
}
