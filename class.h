#ifndef LIL_JVM_CLASS_H
#define LIL_JVM_CLASS_H

#include "common.h"

// https://docs.oracle.com/javase/specs/jvms/se14/html/jvms-4.html
// 4.1. The ClassFile Structure

typedef struct constant_pool_t {
    uint8_t tag;
    // idk what to do with info yet so just make it a byte array for now
    uint8_t **info;
} constant_pool_t;

typedef struct attribute_t {
    uint16_t name_index;
    uint32_t attribute_length;
    // idk what to do with info yet so just make it a byte array for now
    uint8_t **info;
} attribute_t;

typedef struct interface_t {

} interface_t;

typedef struct method_t {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attribute_count;
    attribute_t **attributes;
} method_t;

typedef struct field_t {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attribute_count;
    attribute_t **attributes;
} field_t;

typedef struct class_file_t {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    constant_pool_t **constant_pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    uint16_t interfaces_count;
    interface_t **interfaces;
    uint16_t fields_count;
    field_t **fields;
    uint16_t methods_count;
    method_t **methods;
    uint16_t attributes_count;
    attribute_t **attributes;
} class_file_t;

#endif //LIL_JVM_CLASS_H
