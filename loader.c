#include "loader.h"
#include <stdio.h>
#include "memory.h"


// read unsigned integer into dst

void *current;

static void read_bytes(void *dst, uint16_t count) {
    const void *src = current;
    uint8_t bytes[4];
    memcpy(bytes, src, count);
    switch (count) {
        case 1:
            *(uint8_t *) dst = bytes[0];
            break;
        case 2:
            *(uint16_t *) dst = (bytes[0] << 8) | bytes[1];
            break;
        case 4:
            *(uint32_t *) dst = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
            break;
        default:
            exit(-99);
    }
    current += count;
}

/** * Copy string and append null terminator. */
char * copy_string(const char *chars, int length) {
    char *out = ALLOCATE(char, length + 1);
    memcpy(out, chars, length);
    out[length] = '\0';
    return out;
}

static uint8_t read_byte() {
    uint8_t out;
    read_bytes(&out, 1);
    return out;
}

void read_constant_pool(uint16_t count, constant_pool_t **constant_pool) {
    // this isn't gonna work b/c constant pool entries can be diff size
    // ALLOCATE(constant_pool_t)
    constant_pool_t *cp = ALLOCATE(constant_pool_t, count);
    // constant pool count is number of entries PLUS ONE
    for (uint16_t i = 0; i < count - 1; i++) {
        uint8_t tag = read_byte();
        cp[i].tag = tag;
        switch(tag) {
            case CONSTANT_UTF8:
                read_bytes(&cp[i].info.utf8_info.length, 2);
                cp[i].info.utf8_info.bytes = copy_string(current, cp[i].info.utf8_info.length);
                current += cp[i].info.utf8_info.length;
                break;
            case CONSTANT_CLASS:
                read_bytes(&cp[i].info.class_info.name_index, 2);
                break;
            case CONSTANT_METHOD_REF:
                read_bytes(&cp[i].info.method_ref_info.class_index, 2);
                read_bytes(&cp[i].info.method_ref_info.name_and_type_index, 2);
                break;
            case CONSTANT_NAME_AND_TYPE:
                read_bytes(&cp[i].info.name_and_type_info.name_index, 2);
                read_bytes(&cp[i].info.name_and_type_info.descriptor_index, 2);
                break;
            default:
                printf("Error: Unknown tag 0x%02d for constant pool entry %d", tag, i);
                exit(-1);
        }
    }
    *constant_pool = cp;
    current += count * sizeof(constant_pool_t);
}

void read_interfaces(uint16_t count, interface_t **interfaces) {
    current += count * sizeof(interface_t);
}

void read_fields(uint16_t count, field_t **fields) {
    current += count * sizeof(field_t);
}

void read_methods(uint16_t count, method_t **methods) {
    current += count * sizeof(method_t);
}

void read_attributes(uint16_t count, attribute_t **attribute) {
    current += count * sizeof(attribute_t);
}

void read_class_file(char *bytes, class_file_t *class_file) {
    current = bytes;
    read_bytes(&class_file->magic, 4);
    read_bytes(&class_file->minor_version, 2);
    read_bytes(&class_file->major_version, 2);
    read_bytes(&class_file->constant_pool_count, 2);
    read_constant_pool(class_file->constant_pool_count, &class_file->constant_pool);
    read_bytes(&class_file->access_flags, 2);
    read_bytes(&class_file->this_class, 2);
    read_bytes(&class_file->super_class, 2);
    read_bytes(&class_file->interfaces_count, 2);
    read_interfaces(class_file->interfaces_count, class_file->interfaces);
    read_bytes(&class_file->fields_count, 2);
    read_fields(class_file->fields_count, class_file->fields);
    read_bytes(&class_file->methods_count, 2);
    read_methods(class_file->methods_count, class_file->methods);
    read_bytes(&class_file->attributes_count, 2);
    read_attributes(class_file->attributes_count, class_file->attributes);
}