#ifndef LIL_JVM_CLASS_H
#define LIL_JVM_CLASS_H

#include "common.h"

// only doing ones i need for now
typedef enum constant_tag_t {
    CONSTANT_UTF8          =  1,
    CONSTANT_CLASS         =  7,
    CONSTANT_METHOD_REF    = 10,
    CONSTANT_NAME_AND_TYPE = 12
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
    ATTR_UNKNOWN
} attribute_tag_t;

static char *attribute_tag_map[] = {
        [ATTR_CODE] = "Code",
        [ATTR_CONSTANT_VALUE] = "ConstantValue",
        [ATTR_STACK_MAP_TABLE] = "StackMapTable",
        [ATTR_BOOTSTRAP_METHODS] = "BootstrapMethods",
        [ATTR_NEST_HOST] = "NestHost",
        [ATTR_NEST_MEMBERS] = "NestMembers",
        [ATTR_SOURCE_FILE] = "SourceFile",
        [ATTR_UNKNOWN] = NULL
};

// attribute length refers to length in BYTES of info
typedef struct attribute_t {
    attribute_tag_t tag;
    uint16_t name_index;
    uint32_t attribute_length;
    uint8_t *info;
    // uint8_t tag;
//    union {
//        constant_utf8_info_t utf8_info;
//        constant_class_info_t class_info;
//        constant_method_ref_info_t method_ref_info;
//        constant_name_and_type_info_t name_and_type_info;
//    } info;
} attribute_t;

typedef struct interface_t {

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
    ACC_PUBLIC     = 0x0001,
    ACC_FINAL      = 0x0010,
    ACC_SUPER      = 0x0020,
    ACC_INTERFACE  = 0x0200,
    ACC_ABSTRACT   = 0x0400,
    ACC_SYNTHETIC  = 0x1000,
    ACC_ANNOTATION = 0x2000,
    ACC_ENUM       = 0x4000,
    ACC_MODULE     = 0x8000
} access_flags_t;

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

#endif //LIL_JVM_CLASS_H
