#ifndef LIL_JVM_CLASS_H
#define LIL_JVM_CLASS_H

#include "common.h"
#include "class.h"
#include "common.h"
//#include "memory.h"
#include <string.h>

// 2.6.1 a local variable can hold a value of type boolean, byte,
// char, short, int, float, reference, or return address
// a pair of local variables can hold a value of type long or double.
typedef enum value_type_t {
    VAL_BOOL,
    VAL_INT,
    VAL_LONG,
    VAL_FLOAT,
    VAL_DOUBLE,
    VAL_REF,
    VAL_ADDR,
    // TODO reference
    // TODO return address
} value_type_t;

typedef struct value_t {
    value_type_t type;
    union {
        bool boolean;
        int32_t integer;
        int64_t llong;
        float ffloat;
        double ddouble;
        // TODO reference
        // TODO return address
    } as;
} value_t;

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_INT(value)     ((value).type == VAL_INT)
#define IS_LONG(value)    ((value).type == VAL_LONG)
#define IS_FLOAT(value)   ((value).type == VAL_FLOAT)
#define IS_DOUBLE(value)  ((value).type == VAL_DOUBLE)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_INT(value)     ((value).as.integer)
#define AS_LONG(value)    ((value).as.llong)
#define AS_FLOAT(value)   ((value).as.ffloat)
#define AS_DOUBLE(value)  ((value).as.ddouble)

#define BOOL_VAL(value)  ((value_t) {VAL_BOOL,    {.boolean = value}})
#define INT_VAL(value)   ((value_t) {VAL_INT,     {.integer = value}})
#define LONG_VAL         ((value_t) {VAL_LONG,    {.llong = value}})
#define FLOAT_VAL(value) ((value_t) {VAL_FLOAT,   {.ffloat = value}})
#define DOUBLE_VAL(value) ((value_t) {VAL_DOUBLE, {.ddouble = value}})

typedef enum opcode_t {
    OP_ICONST1 = 0x04,
    OP_ICONST2 = 0x05,
    OP_ILOAD_0 = 0x1a,
    OP_ILOAD_1 = 0x1b,
    OP_ALOAD_0 = 0x2a,
    OP_IADD    = 0x60,
    OP_IRETURN = 0xac,
    OP_RETURN  = 0xb1,
    OP_INVOKE_SPECIAL = 0xb7,
    OP_INVOKE_STATIC = 0xb8
} opcode_t;

// only doing ones i need for now
typedef enum constant_tag_t {
    CONSTANT_UTF8          =  1,
    CONSTANT_CLASS         =  7,
    CONSTANT_METHOD_REF    = 10,
    CONSTANT_NAME_AND_TYPE = 12,
    CONSTANT_UNKNOWN
} constant_tag_t;

//CONSTANT_Utf8_info {
//u1 tag;
//u2 length;
//u1 bytes[length];
//}
typedef struct constant_utf8_info_t {
    uint16_t length;
    char *bytes;
} constant_utf8_info_t;

//CONSTANT_Class_info {
//u1 tag;
//u2 name_index;
//}
typedef struct constant_class_info_t {
    uint16_t name_index;
} constant_class_info_t;

//CONSTANT_Methodref_info {
//u1 tag;
//u2 class_index;
//u2 name_and_type_index;
//}
typedef struct constant_method_ref_info_t {
    uint16_t class_index;
    uint16_t name_and_type_index;
} constant_method_ref_info_t;

//CONSTANT_NameAndType_info {
//u1 tag;
//u2 name_index;
//u2 descriptor_index;
//}
typedef struct constant_name_and_type_info_t {
    uint16_t name_index;
    uint16_t descriptor_index;
} constant_name_and_type_info_t;

typedef struct constant_pool_t {
    uint8_t tag;
    union {
        constant_utf8_info_t utf8_info;
        constant_class_info_t class_info;
        constant_method_ref_info_t method_ref_info;
        constant_name_and_type_info_t name_and_type_info;
    } info;
} constant_pool_t;

//

// these are matched based on string instead of a tag constant
typedef enum attribute_tag_t {
    ATTR_CONSTANT_VALUE,
    ATTR_CODE,
    ATTR_STACK_MAP_TABLE,
    ATTR_BOOTSTRAP_METHODS,
    ATTR_NEST_HOST,
    ATTR_NEST_MEMBERS,
    ATTR_SOURCE_FILE,
    ATTR_LINE_NUM_TABLE,
    ATTR_UNKNOWN
} attribute_tag_t;

typedef struct exception_table_t {
    uint16_t start_pc;
    uint16_t end_pc;
    uint16_t handler_pc;
    uint16_t catch_type;
} exception_table_t;

struct attribute_t;

typedef struct attr_code_t {
    uint16_t max_stack;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t *code;
    uint16_t exception_table_length;
    exception_table_t *exception_table;
    uint16_t attributes_count;
    struct attribute_t *attributes;
} attr_code_t;

typedef struct attr_source_file_t {
    uint16_t sourcefile_index;
} attr_source_file_t;

typedef struct line_number_table_t {
    uint16_t start_pc;
    uint16_t line_number;
} line_number_table_t;

typedef struct attr_line_number_table_t {
    uint16_t line_number_table_length;
    line_number_table_t *line_number_table;
} attr_line_number_table_t;

// attribute length refers to length in BYTES of info
typedef struct attribute_t {
    attribute_tag_t tag;
    uint16_t name_index;
    uint32_t attribute_length;
//    uint8_t *info;
    // uint8_t tag;
    union {
        attr_code_t *attr_code;
        attr_source_file_t *attr_source_file;
        attr_line_number_table_t *attr_line_number_table;
    } info;
} attribute_t;


#define IS_ATTR_CODE(attr)    ((attr).tag == ATTR_CODE)

#define AS_ATTR_CODE(attr)    ((attr)->info.attr_code)

typedef struct interface_t {
    void *todo;
} interface_t;

typedef struct method_t {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attribute_count;
    attribute_t *attributes;
} method_t;

typedef struct field_t {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attribute_count;
    attribute_t *attributes;
} field_t;

typedef enum access_flags_t {
    ACC_FLAG_MIN,
    ACC_PUBLIC     = 0x0001,
    ACC_FINAL      = 0x0010,
    ACC_SUPER      = 0x0020,
    ACC_INTERFACE  = 0x0200,
    ACC_ABSTRACT   = 0x0400,
    ACC_SYNTHETIC  = 0x1000,
    ACC_ANNOTATION = 0x2000,
    ACC_ENUM       = 0x4000,
    ACC_MODULE     = 0x8000,
    ACC_FLAG_MAX
} access_flags_t;

// eehhhh i want to map these to messages but it's kludgy because
// the enum values are like 0, 1, 2, 4, 8, etc. instead of continguous
// array values like 0, 1, 2, 3, 4, 5
//static char *access_flags_map[] = {
//        [ACC_PUBLIC]     = "ACC_PUBLIC",
//        [ACC_FINAL]      = "ACC_FINAL",
//        [ACC_SUPER]      = "ACC_SUPER",
//        [ACC_INTERFACE]  = "ACC_INTERFACE",
//        [ACC_ABSTRACT]   = "ACC_ABSTRACT",
//        [ACC_SYNTHETIC]  = "ACC_SYNTHETIC",
//        [ACC_ANNOTATION] = "ACC_ANNOTATION",
//        [ACC_ENUM]       = "ACC_ENUM",
//        [ACC_MODULE]     = "ACC_MODULE",
//};

// https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html
// 4.1. The ClassFile Structure

// NOTE for constant pool count:
// The value of the constant_pool_count item is equal to the number of entries in the constant_pool table plus one.
// A constant_pool index is considered valid if it is greater than zero and less than constant_pool_count,
// with the exception for constants of type long and double noted in §4.4.5.

typedef struct class_file_t {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    constant_pool_t *constant_pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    uint16_t interfaces_count;
    interface_t *interfaces;
    uint16_t fields_count;
    field_t *fields;
    uint16_t methods_count;
    method_t *methods;
    uint16_t attributes_count;
    attribute_t *attributes;
} class_file_t;

method_t *get_class_method(class_file_t *class, const char *name, const char* descriptor);

method_t *get_methodref(class_file_t *cf, uint16_t index);

attribute_t *get_attribute_by_tag(int16_t attributes_count, attribute_t *attributes, attribute_tag_t tag);

//void read_class_file(char *bytes, class_file_t *class_file);
void read_class_file(uint8_t *bytes, class_file_t *class_file);

char * get_constant_utf8(class_file_t *cf, uint16_t index);
char * get_classname(class_file_t *cf, uint16_t index);

constant_pool_t *get_constant(class_file_t *cf, uint16_t index, constant_tag_t expected);
method_t *get_methodref(class_file_t *cf, uint16_t index);

#endif //LIL_JVM_CLASS_H
