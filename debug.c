#include "debug.h"

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
    if (interfaces == NULL) {
        ;
    }
}

void dump_fields(uint16_t count, field_t *fields) {
    printf("Fields : %u\n", count);
    if (fields == NULL) {
        ;
    }
}

void dump_attributes(class_file_t *class_file, uint16_t count, attribute_t *attributes, int indent_level);


void dump_attribute_common(class_file_t *cf, attribute_t *a, int indent_level) {
    printf("%*s%-20s = 0x%04x // %s\n", indent_level * 2, "", "Name Index", a->name_index,
           get_constant_utf8(cf, a->name_index));
    printf("%*s%-20s = 0x%04x\n", indent_level * 2, "", "Length (Bytes)", a->attribute_length);
}

int simple_inst(const char *name, uint32_t offset) {
    printf("%02u: %s\n", offset, name);
    return offset + 1;
}

void print_constant_info(class_file_t *cf, constant_pool_t *constant) {
    switch (constant->tag) {
        case CONSTANT_FIELDREF:
            printf(" // %s ", get_constant_tag_name(CONSTANT_FIELDREF));
            print_constant_info_at(cf, constant->info.field_ref_info.class_index);
            printf(".");
            print_constant_info_at(cf, constant->info.field_ref_info.name_and_type_index);
            break;
        case CONSTANT_METHOD_REF:
            printf(" // %s ", get_constant_tag_name(CONSTANT_METHOD_REF));
            print_constant_info_at(cf, constant->info.method_ref_info.class_index);
            printf(".");
            print_constant_info_at(cf, constant->info.method_ref_info.name_and_type_index);
            break;
        case CONSTANT_CLASS:
            print_constant_info_at(cf, constant->info.class_info.name_index);
            break;
        case CONSTANT_NAME_AND_TYPE:
            print_constant_info_at(cf, constant->info.name_and_type_info.name_index);
            printf(":");
            print_constant_info_at(cf, constant->info.name_and_type_info.descriptor_index);
            break;
        case CONSTANT_UTF8:
            printf("%s", constant->info.utf8_info.bytes);
            break;
        case CONSTANT_STRING:
            printf(" // %s ", get_constant_tag_name(CONSTANT_STRING));
            print_constant_info_at(cf, constant->info.string_info.string_index);
            break;
        default:
            printf(" // %s TODO ADD DEBUG PRINT ", get_constant_tag_name(constant->tag));
    }
}

void print_constant_info_at(class_file_t *cf, uint16_t index) {
    constant_pool_t *constant = get_constant(cf, index);
    switch (constant->tag) {
        case CONSTANT_FIELDREF:
            printf(" // %s ", get_constant_tag_name(CONSTANT_FIELDREF));
            print_constant_info_at(cf, constant->info.field_ref_info.class_index);
            printf(".");
            print_constant_info_at(cf, constant->info.field_ref_info.name_and_type_index);
            break;
        case CONSTANT_METHOD_REF:
            printf(" // %s ", get_constant_tag_name(CONSTANT_METHOD_REF));
            print_constant_info_at(cf, constant->info.method_ref_info.class_index);
            printf(".");
            print_constant_info_at(cf, constant->info.method_ref_info.name_and_type_index);
            break;
        case CONSTANT_CLASS:
            print_constant_info_at(cf, constant->info.class_info.name_index);
            break;
        case CONSTANT_NAME_AND_TYPE:
            print_constant_info_at(cf, constant->info.name_and_type_info.name_index);
            printf(":");
            print_constant_info_at(cf, constant->info.name_and_type_info.descriptor_index);
            break;
        case CONSTANT_UTF8:
            printf("%s", get_constant_utf8(cf, index));
            break;
        case CONSTANT_STRING:
            printf(" // %s ", get_constant_tag_name(CONSTANT_STRING));
            print_constant_info_at(cf, constant->info.string_info.string_index);
            break;
        default:
            printf(" // %s TODO ADD DEBUG PRINT ", get_constant_tag_name(constant->tag));
    }
}

// instructions that have one byte index
int byte_index_inst(class_file_t *cf, const char *name, uint8_t *code, uint32_t offset) {
    uint8_t index = code[offset + 1];
    printf("%02u: %-20s #%4u ", offset, name, index);
    print_constant_info_at(cf, index);
    printf("\n");
    return offset + 2;
}

// instructions that have two byte index
int word_index_inst(class_file_t *cf, const char *name, uint8_t *code, uint32_t offset) {
    uint16_t index = (code[offset + 1] << 8) | (code[offset + 2]);
    printf("%02u: %-20s #%4u ", offset, name, index);
    print_constant_info_at(cf, index);
    printf("\n");
    return offset + 3;
}

int disassemble_inst(class_file_t *cf, uint8_t *code, uint32_t offset, int indent_level) {
    uint8_t opcode = code[offset];
    printf("%*s", indent_level * 2, "");
    printf("(0x%02x) ", opcode);
    switch(opcode) {
        case OP_ICONST1:
            return simple_inst("iconst1", offset);
        case OP_ICONST2:
            return simple_inst("iconst2", offset);
        case OP_LDC:
            return byte_index_inst(cf, "ldc", code, offset);
        case OP_ALOAD_0:
            return simple_inst("aload_0", offset);
        case OP_GET_STATIC:
            return word_index_inst(cf, "getstatic", code, offset);
        case OP_INVOKE_VIRTUAL:
            return word_index_inst(cf, "invokevirtual", code, offset);
        case OP_INVOKE_SPECIAL:
            return word_index_inst(cf, "invokespecial", code, offset);
        case OP_INVOKE_STATIC:
            return word_index_inst(cf, "invokestatic", code, offset);
        case OP_ILOAD_0:
            return simple_inst("iload_0", offset);
        case OP_ILOAD_1:
            return simple_inst("iload_1", offset);
        case OP_IADD:
            return simple_inst("iadd", offset);
        case OP_IRETURN:
            return simple_inst("ireturn", offset);
        case OP_RETURN:
            return simple_inst("return", offset);
        default:
            printf("Unknown opcode %d\n", opcode);
            return offset + 1;
    }
}

void disassemble_code(class_file_t *cf, uint8_t *code, uint32_t code_length, int indent_level) {
    for (uint32_t offset = 0; offset < code_length;) {
        offset = disassemble_inst(cf, code, offset, indent_level + 1);
    }
}

void dump_attr_code(class_file_t *cf, attribute_t *attr, int indent_level) {
    printf("%*sCode Attribute :\n", indent_level * 2, "");
    dump_attribute_common(cf, attr, indent_level + 1);
    attr_code_t *code_attr = attr->info.attr_code;
    printf("%*s%-20s = 0x%04d\n", (indent_level + 1) * 2, "", "Max Stack", code_attr->max_stack);
    printf("%*s%-20s = 0x%04d\n", (indent_level + 1) * 2, "", "Max Locals", code_attr->max_locals);
    disassemble_code(cf, code_attr->code, code_attr->code_length, indent_level + 1);
    printf("%*sException Table : %u\n", (indent_level + 0) * 2, "", code_attr->exception_table_length);
    for (uint16_t i = 0; i < code_attr->exception_table_length; i++) {
        printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "Start PC", code_attr->exception_table[i].start_pc);
        printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "End PC",  code_attr->exception_table[i].start_pc);
        printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "Handler PC",  code_attr->exception_table[i].handler_pc);
        printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "Catch Type",  code_attr->exception_table[i].catch_type);
    }
}

void dump_attr_source_file(class_file_t *cf, attribute_t *attr, int indent_level) {
    attr_source_file_t *source_attr = attr->info.attr_source_file;
    printf("%*sSource File Attribute :\n", (indent_level + 0) * 2, "");
    dump_attribute_common(cf, attr, indent_level + 1);
    printf("%*s%-20s = 0x%04x // %s\n",
           (indent_level + 1) * 2, "",
           "Source File Index",
           source_attr->sourcefile_index,
           get_constant_utf8(cf, source_attr->sourcefile_index));
}

void dump_attr_line_num_table(class_file_t *cf, attribute_t *attr, int indent_level) {
    printf("%*sLine Number Table :\n", indent_level * 2, "");
    dump_attribute_common(cf, attr, indent_level + 1);
    attr_line_number_table_t *a = attr->info.attr_line_number_table;
    printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "Line Number Table Length", a->line_number_table_length);
    for (uint16_t i = 0; i < a->line_number_table_length; i++) {
        printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "Start PC", a->line_number_table[i].start_pc);
        printf("%*s%-20s = %u\n", (indent_level + 1) * 2, "",     "Line Number", a->line_number_table[i].line_number);
    }
}

void dump_attributes(class_file_t *class_file, uint16_t count, attribute_t *attributes, int indent_level) {
    printf("%*sAttributes : %u\n", indent_level * 2, "", count);
    for (uint16_t i = 0; i < count; i++) {
        switch(attributes[i].tag) {
            case ATTR_CODE:
                dump_attr_code(class_file, &attributes[i], indent_level + 1);
                break;
            case ATTR_LINE_NUM_TABLE:
                dump_attr_line_num_table(class_file, &attributes[i], indent_level + 1);
                break;
            case ATTR_SOURCE_FILE:
                dump_attr_source_file(class_file, &attributes[i], indent_level + 1);
                break;
            case ATTR_UNKNOWN:
            default:
                printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "Unknown Attribute", attributes[i].tag);
                break;
        }
    }
}

void dump_methods(class_file_t *class_file, uint16_t count, method_t *methods) {
    //class_file_t *class_file;
    printf("Methods : %u\n", count);
    for (uint16_t i = 0; i < count; i++) {
        printf("  Method[%u]:\n", i);
        printf("    %-20s = 0x%04x\n", "Access Flags",     methods[i].access_flags);
        printf("    %-20s = 0x%04x // %s\n", "Name Index", methods[i].name_index,
               get_constant_utf8(class_file, methods[i].name_index));
//        printf("    %-20s = 0x%04x // %s\n", "Descriptor Index", methods[i].descriptor_index,
//               get_constant_utf8(class_file, methods[i].descriptor_index));
        printf("    %-20s = 0x%04x\n", "Descriptor Index", methods[i].descriptor_index);
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
