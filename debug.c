#include "debug.h"

void dump_constant_pool(uint16_t count, constant_pool_t **constant_pool) {
    printf("Constant Pool:\n");
}

void dump_interfaces(uint16_t count, interface_t **interfaces) {
    printf("Interfaces:\n");
}

void dump_fields(uint16_t count, field_t **fields) {
    printf("Fields:\n");
}

void dump_methods(uint16_t count, method_t **methods) {
    printf("Methods:\n");
}

void dump_attributes(uint16_t count, attribute_t **attribute) {
    printf("Attributes:\n");
}

void dump_classfile(class_file_t *class_file) {
    printf("%-25s 0x%08x '\n", "Magic Number", class_file->magic);
    printf("%-25s 0x%04x '\n", "Minor Version", class_file->minor_version);
    printf("%-25s 0x%04x '\n", "Major Version", class_file->major_version);
    printf("%-25s 0x%04x '\n", "Constant Pool Count", class_file->constant_pool_count);
    dump_constant_pool(class_file->constant_pool_count, class_file->constant_pool);
    printf("%-25s 0x%04x '\n", "Access Flags", class_file->access_flags);
    printf("%-25s 0x%04x '\n", "This class", class_file->this_class);
    printf("%-25s 0x%04x '\n", "Super class", class_file->super_class);
    printf("%-25s 0x%04x '\n", "Interfaces Count", class_file->interfaces_count);
    dump_interfaces(class_file->interfaces_count, class_file->interfaces);
    dump_fields(class_file->fields_count, class_file->fields);
    dump_methods(class_file->methods_count, class_file->methods);
    dump_attributes(class_file->attributes_count, class_file->attributes);
}
