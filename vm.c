#include "vm.h"
#include "common.h"
#include "memory.h"
#include "debug.h"
#include "class.h"

vm_t vm;

void print_value(value_t value) {
    switch(value.type) {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_INT:    printf("%d",  AS_INT(value)); break;
        case VAL_LONG:   printf("%ld", AS_LONG(value)); break;
        case VAL_FLOAT:  printf("%f",  AS_FLOAT(value)); break;
        case VAL_DOUBLE: printf("%f",  AS_DOUBLE(value)); break;
    }
}

void push(value_t value) {
    *vm.stack_top = value;
    vm.stack_top++;
}

value_t pop() {
    vm.stack_top--;
    return *vm.stack_top;
}

static value_t peek(int distance) {
    return vm.stack_top[-1 - distance];
}

//frame_t *push_frame(frame_t *parent, uint8_t *code, class_file_t *class_file, uint16_t max_stack, uint16_t max_locals) {
frame_t *push_frame(uint8_t *code, class_file_t *class_file, uint16_t max_stack, uint16_t max_locals) {
    frame_t *frame = ALLOCATE(frame_t, 1);

    frame->class_file = class_file;
    frame->locals = ALLOCATE(value_t, max_locals);
    frame->stack = ALLOCATE(value_t, max_stack);
    frame->stack_top = frame->stack;
    frame->code = code;
    frame->pc = 0;

    return frame;
}

static interpret_result_t run() {
    frame_t *frame = vm.frames;
    frame->stack = vm.stack;
    frame->stack_top = vm.stack_top;
//#define READ_BYTE() (*frame[->ip++)
#define READ_BYTE() (frame->code[frame->pc++])
    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
    for (value_t *slot = vm.stack; slot < vm.stack_top; slot++) {
      printf("[ ");
      print_value(*slot);
      printf(" ]");
    }
    printf("\n");
    disassemble_inst(frame->code, frame->pc, 0);
#endif
        uint8_t inst;
        switch (inst = READ_BYTE()) {
            case OP_ICONST1:
                push(INT_VAL(1)); break;
            case OP_ICONST2:
                push(INT_VAL(2)); break;
//            case OP_ALOAD_0:
//                return simple_inst("aload_0", offset);
//            case OP_INVOKE_SPECIAL:
//                return invoke_inst("invokespecial", code, offset);
//            case OP_INVOKE_STATIC:
//                return invoke_inst("invokestatic", code, offset);
//            case OP_ILOAD_0:
//                return simple_inst("iload_0", offset);
//            case OP_ILOAD_1:
//                return simple_inst("iload_1", offset);
//            case OP_IADD:
//                return simple_inst("iadd", offset);
//            case OP_IRETURN:
//                return simple_inst("ireturn", offset);
//            case OP_RETURN:
//                return simple_inst("return", offset);
            default:
                printf("Unknown opcode %d\n", inst);
                return INTERPRET_RUNTIME_ERROR;
        }
    }
}

static void reset_stack() {
    vm.stack_top = vm.stack;
    vm.frame_count = 0;
}

interpret_result_t interpret(class_file_t *class_file) {
    reset_stack();
    // hard coding this for one class file (AddMain.class)
    attr_code_t *code_attr = class_file->methods[2].attributes[0].info.attr_code;
    frame_t *frame = push_frame(code_attr->code, class_file, 2, 1);
    vm.frames[0] = *frame;
    vm.stack_top = vm.stack;
//    frame_t *frame = push_frame(NULL, code_attr->code, class_file, 2, 1);

    return run();
}

