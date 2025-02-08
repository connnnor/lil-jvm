#include "loader.h"


// read unsigned integer into dst

void *current;

static void read_bytes(const void *src, void *dst, uint16_t count) {
    uint8_t bytes[4];
    memcpy(bytes, src, count);
    switch (count) {
        case 1:
            *(uint8_t *) dst = bytes[0];
            break;
        case 2:
            *(uint16_t *) dst = (bytes[1] << 8) | bytes[0];
            break;
        case 4:
            *(uint32_t *) dst = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
            break;
        default:
            exit(-99);
    }
    current += count;
}

void read_constant_pool(uint16_t count, constant_pool_t **constant_pool) {
    // this isn't gonna work b/c constant pool entries can be diff size
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
    read_bytes(current, &class_file->magic, 4);
    read_bytes(current, &class_file->minor_version, 2);
    read_bytes(current, &class_file->major_version, 2);
    read_bytes(current, &class_file->constant_pool_count, 2);
    read_constant_pool(class_file->constant_pool_count, class_file->constant_pool);
    read_bytes(current, &class_file->access_flags, 2);
    read_bytes(current, &class_file->this_class, 2);
    read_bytes(current, &class_file->super_class, 2);
    read_bytes(current, &class_file->interfaces_count, 2);
    read_interfaces(class_file->interfaces_count, class_file->interfaces);
    read_bytes(current, &class_file->fields_count, 2);
    read_fields(class_file->fields_count, class_file->fields);
    read_bytes(current, &class_file->methods_count, 2);
    read_methods(class_file->methods_count, class_file->methods);
    read_bytes(current, &class_file->attributes_count, 2);
    read_attributes(class_file->attributes_count, class_file->attributes);
}