#include "debug.h"
#include "loader.h"

void print_bytes(unsigned char *b, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        printf("0x%02x", b[i]);
        if (i != length - 1) {
            printf(", ");
        }
    }
}

void dump_constant_pool(uint16_t count, constant_pool_t *cp) {
    printf("Constant Pool : %u\n", count - 1);
    for (uint16_t i = 0; i < count - 1; i++) {
        printf("  #%02u = ", i + 1);
        switch (cp[i].tag) {
            case CONSTANT_UTF8:
                printf("%-20s ", "Utf8");
                // FIXME - should probably do something if this string is long like a "..."
                printf("%s ", cp[i].info.utf8_info.bytes);
                break;
            case CONSTANT_CLASS:
                printf("%-20s ", "Class");
                printf("#%d", cp[i].info.class_info.name_index);
                break;
            case CONSTANT_METHOD_REF:
                printf("%-20s ", "Methodref");
                printf("#%d.#%d", cp[i].info.method_ref_info.class_index, cp[i].info.method_ref_info.name_and_type_index);
                break;
            case CONSTANT_NAME_AND_TYPE:
                printf("%-20s ", "NameAndType");
                printf("#%d:#%d", cp[i].info.name_and_type_info.name_index, cp[i].info.name_and_type_info.descriptor_index);
                break;
            default:
                printf("Unknown tag 0x%02d", cp[i].tag);
        }
        printf("\n");
    }
}

void dump_interfaces(uint16_t count, interface_t *interfaces) {
    printf("Interfaces : %u\n", count);
}

void dump_fields(uint16_t count, field_t *fields) {
    printf("Fields : %u\n", count);
}

void dump_attributes(class_file_t *class_file, uint16_t count, attribute_t *attributes, int indent_level) {
    printf("%*sAttributes : %u\n", indent_level * 2, "", count);
    for (uint16_t i = 0; i < count; i++) {
        printf("%*s%-20s = 0x%04x // %s\n", (indent_level + 1) * 2, "", "Name Index", attributes[i].name_index,
               get_constant_utf8(class_file, attributes[i].name_index));
        printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "Length (Bytes)", attributes[i].attribute_length);
        //printf("%*s%-20s = %s", (indent_level + 1) * 2, "", "Info", "TODO (");
        //print_bytes(attributes[i].info, attributes[i].attribute_length);
        //printf(")\n");
    }
}

void dump_methods(class_file_t *class_file, uint16_t count, method_t *methods) {
    //class_file_t *class_file;
    printf("Methods : %u\n", count);
    for (uint16_t i = 0; i < count; i++) {
        printf("  Method[%u]:\n", i);
        printf("    %-20s = 0x%04x\n", "Access Flags",     methods[i].access_flags);
        uint16_t name_index = methods[i].name_index;
        printf("    %-20s = 0x%04x // %s\n", "Name Index", methods[i].name_index,
               get_constant_utf8(class_file, methods[i].name_index));
        printf("    %-20s = 0x%04x // %s\n", "Descriptor Index", methods[i].descriptor_index,
               get_constant_utf8(class_file, methods[i].descriptor_index));
        dump_attributes(class_file, methods[i].attribute_count, methods[i].attributes, 2);
    }
}

void dump_classfile(class_file_t *class_file) {
    printf("%-25s 0x%08x '\n", "Magic Number", class_file->magic);
    printf("%-25s %4d '\n", "Minor Version", class_file->minor_version);
    printf("%-25s %4d '\n", "Major Version", class_file->major_version);
    dump_constant_pool(class_file->constant_pool_count, class_file->constant_pool);
    printf("%-25s 0x%04x '\n", "Access Flags", class_file->access_flags);
    printf("%-25s 0x%04x // %s'\n", "This class", class_file->this_class, get_classname(class_file, class_file->this_class));
    printf("%-25s 0x%04x '\n", "Super class", class_file->super_class);
    dump_interfaces(class_file->interfaces_count, class_file->interfaces);
    dump_fields(class_file->fields_count, class_file->fields);
    dump_methods(class_file, class_file->methods_count, class_file->methods);
    dump_attributes(class_file, class_file->attributes_count, class_file->attributes, 0);
}
