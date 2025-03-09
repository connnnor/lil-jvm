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
    VAL_ADDR
} value_type_t;

struct constant_pool_t;

typedef struct value_t {
    value_type_t type;
    union {
        bool boolean;
        int32_t integer;
        int64_t llong;
        float ffloat;
        double ddouble;
        struct constant_pool_t *reference;
        // TODO reference
        // TODO return address
    } as;
} value_t;

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_INT(value)     ((value).type == VAL_INT)
#define IS_LONG(value)    ((value).type == VAL_LONG)
#define IS_FLOAT(value)   ((value).type == VAL_FLOAT)
#define IS_DOUBLE(value)  ((value).type == VAL_DOUBLE)
#define IS_REFERENCE(value)  ((value).type == VAL_REF)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_INT(value)     ((value).as.integer)
#define AS_LONG(value)    ((value).as.llong)
#define AS_FLOAT(value)   ((value).as.ffloat)
#define AS_DOUBLE(value)  ((value).as.ddouble)
#define AS_REFERENCE(value)  ((value).as.reference)

#define BOOL_VAL(value)  ((value_t) {VAL_BOOL,    {.boolean = value}})
#define INT_VAL(value)   ((value_t) {VAL_INT,     {.integer = value}})
#define LONG_VAL         ((value_t) {VAL_LONG,    {.llong = value}})
#define FLOAT_VAL(value) ((value_t) {VAL_FLOAT,   {.ffloat = value}})
#define DOUBLE_VAL(value) ((value_t) {VAL_DOUBLE, {.ddouble = value}})
#define REFERENCE_VAL(value) ((value_t) {VAL_REF, {.reference = value}})

typedef enum opcode_t {
    OP_NOP              = 0x00,
    OP_ACONST_NULL      = 0x01,
    OP_ICONST_M1        = 0x02,
    OP_ICONST_0         = 0x03,
    OP_ICONST_1         = 0x04,
    OP_ICONST_2         = 0x05,
    OP_ICONST_3         = 0x06,
    OP_ICONST_4         = 0x07,
    OP_ICONST_5         = 0x08,
    OP_LCONST_0         = 0x09,
    OP_LCONST_1         = 0x0a,
    OP_FCONST_0         = 0x0b,
    OP_FCONST_1         = 0x0c,
    OP_FCONST_2         = 0x0d,
    OP_DCONST_0         = 0x0e,
    OP_DCONST_1         = 0x0f,
    OP_BIPUSH           = 0x10,
    OP_SIPUSH           = 0x11,
    OP_LDC              = 0x12,
    OP_LDC_W            = 0x13,
    OP_LDC2_W           = 0x14,
    OP_ILOAD            = 0x15,
    OP_LLOAD            = 0x16,
    OP_FLOAD            = 0x17,
    OP_DLOAD            = 0x18,
    OP_ALOAD            = 0x19,
    OP_ILOAD_0          = 0x1a,
    OP_ILOAD_1          = 0x1b,
    OP_ILOAD_2          = 0x1c,
    OP_ILOAD_3          = 0x1d,
    OP_LLOAD_0          = 0x1e,
    OP_LLOAD_1          = 0x1f,
    OP_LLOAD_2          = 0x20,
    OP_LLOAD_3          = 0x21,
    OP_FLOAD_0          = 0x22,
    OP_FLOAD_1          = 0x23,
    OP_FLOAD_2          = 0x24,
    OP_FLOAD_3          = 0x25,
    OP_DLOAD_0          = 0x26,
    OP_DLOAD_1          = 0x27,
    OP_DLOAD_2          = 0x28,
    OP_DLOAD_3          = 0x29,
    OP_ALOAD_0          = 0x2a,
    OP_ALOAD_1          = 0x2b,
    OP_ALOAD_2          = 0x2c,
    OP_ALOAD_3          = 0x2d,
    OP_IALOAD           = 0x2e,
    OP_LALOAD           = 0x2f,
    OP_FALOAD           = 0x30,
    OP_DALOAD           = 0x31,
    OP_AALOAD           = 0x32,
    OP_BALOAD           = 0x33,
    OP_CALOAD           = 0x34,
    OP_SALOAD           = 0x35,
    OP_ISTORE           = 0x36,
    OP_LSTORE           = 0x37,
    OP_FSTORE           = 0x38,
    OP_DSTORE           = 0x39,
    OP_ASTORE           = 0x3a,
    OP_ISTORE_0         = 0x3b,
    OP_ISTORE_1         = 0x3c,
    OP_ISTORE_2         = 0x3d,
    OP_ISTORE_3         = 0x3e,
    OP_LSTORE_0         = 0x3f,
    OP_LSTORE_1         = 0x40,
    OP_LSTORE_2         = 0x41,
    OP_LSTORE_3         = 0x42,
    OP_FSTORE_0         = 0x43,
    OP_FSTORE_1         = 0x44,
    OP_FSTORE_2         = 0x45,
    OP_FSTORE_3         = 0x46,
    OP_DSTORE_0         = 0x47,
    OP_DSTORE_1         = 0x48,
    OP_DSTORE_2         = 0x49,
    OP_DSTORE_3         = 0x4a,
    OP_ASTORE_0         = 0x4b,
    OP_ASTORE_1         = 0x4c,
    OP_ASTORE_2         = 0x4d,
    OP_ASTORE_3         = 0x4e,
    OP_IASTORE          = 0x4f,
    OP_LASTORE          = 0x50,
    OP_FASTORE          = 0x51,
    OP_DASTORE          = 0x52,
    OP_AASTORE          = 0x53,
    OP_BASTORE          = 0x54,
    OP_CASTORE          = 0x55,
    OP_SASTORE          = 0x56,
    OP_POP              = 0x57,
    OP_POP2             = 0x58,
    OP_DUP              = 0x59,
    OP_DUP_X1           = 0x5a,
    OP_DUP_X2           = 0x5b,
    OP_DUP2             = 0x5c,
    OP_DUP2_X1          = 0x5d,
    OP_DUP2_X2          = 0x5e,
    OP_SWAP             = 0x5f,
    OP_IADD             = 0x60,
    OP_LADD             = 0x61,
    OP_FADD             = 0x62,
    OP_DADD             = 0x63,
    OP_ISUB             = 0x64,
    OP_LSUB             = 0x65,
    OP_FSUB             = 0x66,
    OP_DSUB             = 0x67,
    OP_IMUL             = 0x68,
    OP_LMUL             = 0x69,
    OP_FMUL             = 0x6a,
    OP_DMUL             = 0x6b,
    OP_IDIV             = 0x6c,
    OP_LDIV             = 0x6d,
    OP_FDIV             = 0x6e,
    OP_DDIV             = 0x6f,
    OP_IREM             = 0x70,
    OP_LREM             = 0x71,
    OP_FREM             = 0x72,
    OP_DREM             = 0x73,
    OP_INEG             = 0x74,
    OP_LNEG             = 0x75,
    OP_FNEG             = 0x76,
    OP_DNEG             = 0x77,
    OP_ISHL             = 0x78,
    OP_LSHL             = 0x79,
    OP_ISHR             = 0x7a,
    OP_LSHR             = 0x7b,
    OP_IUSHR            = 0x7c,
    OP_LUSHR            = 0x7d,
    OP_IAND             = 0x7e,
    OP_LAND             = 0x7f,
    OP_IOR              = 0x80,
    OP_LOR              = 0x81,
    OP_IXOR             = 0x82,
    OP_LXOR             = 0x83,
    OP_IINC             = 0x84,
    OP_I2L              = 0x85,
    OP_I2F              = 0x86,
    OP_I2D              = 0x87,
    OP_L2I              = 0x88,
    OP_L2F              = 0x89,
    OP_L2D              = 0x8a,
    OP_F2I              = 0x8b,
    OP_F2L              = 0x8c,
    OP_F2D              = 0x8d,
    OP_D2I              = 0x8e,
    OP_D2L              = 0x8f,
    OP_D2F              = 0x90,
    OP_I2B              = 0x91,
    OP_I2C              = 0x92,
    OP_I2S              = 0x93,
    OP_LCMP             = 0x94,
    OP_FCMPL            = 0x95,
    OP_FCMPG            = 0x96,
    OP_DCMPL            = 0x97,
    OP_DCMPG            = 0x98,
    OP_IFEQ             = 0x99,
    OP_IFNE             = 0x9a,
    OP_IFLT             = 0x9b,
    OP_IFGE             = 0x9c,
    OP_IFGT             = 0x9d,
    OP_IFLE             = 0x9e,
    OP_IF_ICMPEQ        = 0x9f,
    OP_IF_ICMPNE        = 0xa0,
    OP_IF_ICMPLT        = 0xa1,
    OP_IF_ICMPGE        = 0xa2,
    OP_IF_ICMPGT        = 0xa3,
    OP_IF_ICMPLE        = 0xa4,
    OP_IF_ACMPEQ        = 0xa5,
    OP_IF_ACMPNE        = 0xa6,
    OP_GOTO             = 0xa7,
    OP_JSR              = 0xa8,
    OP_RET              = 0xa9,
    OP_TABLESWITCH      = 0xaa,
    OP_LOOKUPSWITCH     = 0xab,
    OP_IRETURN          = 0xac,
    OP_LRETURN          = 0xad,
    OP_FRETURN          = 0xae,
    OP_DRETURN          = 0xaf,
    OP_ARETURN          = 0xb0,
    OP_RETURN           = 0xb1,
    OP_GETSTATIC        = 0xb2,
    OP_PUTSTATIC        = 0xb3,
    OP_GETFIELD         = 0xb4,
    OP_PUTFIELD         = 0xb5,
    OP_INVOKEVIRTUAL    = 0xb6,
    OP_INVOKESPECIAL    = 0xb7,
    OP_INVOKESTATIC     = 0xb8,
    OP_INVOKEINTERFACE  = 0xb9,
    OP_INVOKEDYNAMIC    = 0xba,
    OP_NEW              = 0xbb,
    OP_NEWARRAY         = 0xbc,
    OP_ANEWARRAY        = 0xbd,
    OP_ARRAYLENGTH      = 0xbe,
    OP_ATHROW           = 0xbf,
    OP_CHECKCAST        = 0xc0,
    OP_INSTANCEOF       = 0xc1,
    OP_MONITORENTER     = 0xc2,
    OP_MONITOREXIT      = 0xc3,
    OP_WIDE             = 0xc4,
    OP_MULTIANEWARRAY   = 0xc5,
    OP_IFNULL           = 0xc6,
    OP_IFNONNULL        = 0xc7,
    OP_GOTO_W           = 0xc8,
    OP_JSR_W            = 0xc9,
    OP_BREAKPOINT       = 0xca,
    OP_IMPDEP1          = 0xfe,
    OP_IMPDEP2          = 0xff
} opcode_t;


// only doing ones i need for now
typedef enum constant_tag_t {
    CONSTANT_CLASS = 7,
    CONSTANT_FIELDREF = 9,
    CONSTANT_METHOD_REF           = 10,
    CONSTANT_INTERFACE_METHOD_REF = 11,
    CONSTANT_STRING               =  8,
    CONSTANT_INTEGER              =  3,
    CONSTANT_FLOAT                =  4,
    CONSTANT_LONG                 =  5,
    CONSTANT_DOUBLE               =  6,
    CONSTANT_NAME_AND_TYPE        = 12,
    CONSTANT_UTF8                 =  1,
    CONSTANT_METHOD_HANDLE        = 15,
    CONSTANT_METHOD_TYPE          = 16,
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

typedef struct constant_string_info_t {
    uint16_t string_index;
} constant_string_info_t;

//CONSTANT_Fieldref_info {
//  u1 tag;
//  u2 class_index;
//  u2 name_and_type_index;
//}

typedef struct constant_field_ref_info_t {
    uint16_t class_index;
    uint16_t name_and_type_index;
} constant_field_ref_info_t;

typedef struct constant_pool_t {
//    uint8_t tag;
    constant_tag_t tag;
    union {
        constant_utf8_info_t utf8_info;
        constant_class_info_t class_info;
        constant_method_ref_info_t method_ref_info;
        constant_field_ref_info_t field_ref_info;
        constant_name_and_type_info_t name_and_type_info;
        constant_string_info_t string_info;
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

typedef struct attr_stack_map_table_t {
    uint16_t number_of_entries;
    uint8_t *entries;
} attr_stack_map_table_t;

typedef struct line_number_table_t {
    uint16_t start_pc;
    uint16_t line_number;
} line_number_table_t;

typedef struct attr_line_number_table_t {
    uint16_t line_number_table_length;
    line_number_table_t *line_number_table;
} attr_line_number_table_t;

typedef struct attr_nestmembers_t {
    uint16_t number_of_classes;
    uint16_t* classes; // constant pool index. must point to constant_Class_info structure
} attr_nestmembers_t;

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
        attr_stack_map_table_t *attr_stack_map_table;
        attr_nestmembers_t *attr_nestmembers;
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
    ACC_PRIVATE    = 0x0002,
    ACC_PROTECTED  = 0x0004,
    ACC_STATIC     = 0x0008,
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

constant_pool_t *get_constant(class_file_t *cf, uint16_t index);

constant_pool_t *get_constant_exp(class_file_t *cf, uint16_t index, constant_tag_t expected);
method_t *get_methodref(class_file_t *cf, uint16_t index);

char *get_constant_tag_name(constant_tag_t tag);

char *get_value_tag_name(value_type_t value_type);

typedef struct reference_t {
    //constant_tag_t tag;
    constant_pool_t *constant;
} reference_t;

#endif //LIL_JVM_CLASS_H
