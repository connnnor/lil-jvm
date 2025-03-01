#include "debug.h"

void print_bytes(unsigned char *b, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        printf("0x%02x", b[i]);
        if (i != length - 1) {
            printf(", ");
        }
    }
}

void dump_constant_pool(class_file_t *cf, uint16_t count, constant_pool_t *cp) {
    printf("Constant Pool : %u\n", count - 1);
    for (uint16_t i = 0; i < count - 1; i++) {
        printf("  #%02u = ", i + 1);
        printf("%-18s", get_constant_tag_name(cp[i].tag));
        switch (cp[i].tag) {
            case CONSTANT_UTF8:
                break;
            case CONSTANT_CLASS:
                printf("#%3d", cp[i].info.class_info.name_index);
                printf("%-16s", "");
                printf("// ");
                break;
            case CONSTANT_STRING:
                printf("#%3d", cp[i].info.string_info.string_index);
                printf("%-16s", "");
                printf("// ");
                break;
            case CONSTANT_METHOD_REF:
                printf("#%3d.#%3d", cp[i].info.method_ref_info.class_index, cp[i].info.method_ref_info.name_and_type_index);
                printf("%-11s", "");
                printf("// ");
                break;
            case CONSTANT_NAME_AND_TYPE:
                printf("#%3d:#%3d", cp[i].info.name_and_type_info.name_index, cp[i].info.name_and_type_info.descriptor_index);
                printf("%-11s", "");
                printf("// ");
                break;
            default:
                printf("Unknown tag 0x%02d", cp[i].tag);
                break;
        }
        print_constant_info_at(cf, i + 1);
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

int simple_inst(opcode_t opcode, uint32_t offset) {
    printf("%02u: %s\n", offset, get_opcode_name(opcode));
    return offset + 1;
}

void print_constant_info(class_file_t *cf, constant_pool_t *constant) {
    switch (constant->tag) {
        case CONSTANT_FIELDREF:
            printf("// %s ", get_constant_tag_name(CONSTANT_FIELDREF));
            print_constant_info_at(cf, constant->info.field_ref_info.class_index);
            printf(".");
            print_constant_info_at(cf, constant->info.field_ref_info.name_and_type_index);
            break;
        case CONSTANT_METHOD_REF:
            printf("// %s ", get_constant_tag_name(CONSTANT_METHOD_REF));
            print_constant_info_at(cf, constant->info.method_ref_info.class_index);
            printf(".");
            print_constant_info_at(cf, constant->info.method_ref_info.name_and_type_index);
            break;
        case CONSTANT_CLASS:
            print_constant_info_at(cf, constant->info.class_info.name_index);
            break;
        case CONSTANT_NAME_AND_TYPE:
            printf("// ");
            print_constant_info_at(cf, constant->info.name_and_type_info.name_index);
            printf(":");
            print_constant_info_at(cf, constant->info.name_and_type_info.descriptor_index);
            break;
        case CONSTANT_UTF8:
            printf("%s", constant->info.utf8_info.bytes);
            break;
        case CONSTANT_STRING:
            printf("// %s ", get_constant_tag_name(CONSTANT_STRING));
            print_constant_info_at(cf, constant->info.string_info.string_index);
            break;
        default:
            printf("// %s TODO ADD DEBUG PRINT ", get_constant_tag_name(constant->tag));
    }
}

void print_constant_info_at(class_file_t *cf, uint16_t index) {
    constant_pool_t *constant = get_constant(cf, index);
    switch (constant->tag) {
        case CONSTANT_FIELDREF:
            print_constant_info_at(cf, constant->info.field_ref_info.class_index);
            printf(".");
            print_constant_info_at(cf, constant->info.field_ref_info.name_and_type_index);
            break;
        case CONSTANT_METHOD_REF:
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
            print_constant_info_at(cf, constant->info.string_info.string_index);
            break;
        default:
            printf("// %s TODO ADD DEBUG PRINT ", get_constant_tag_name(constant->tag));
    }
}

char *get_opcode_name(opcode_t opcode) {
    return opcode_name_map[opcode];
}

// instructions that have two byte args
int two_byte_value_inst(opcode_t opcode, uint8_t *code, uint32_t offset) {
    uint8_t index = code[offset + 1];
    uint8_t byte = code[offset + 1];
    printf("%02u: %-20s #%4u, %d\n", offset, get_opcode_name(opcode), index, byte);
    return offset + 3;
}

// instructions that have one byte index
int byte_index_inst(class_file_t *cf, opcode_t opcode, uint8_t *code, uint32_t offset) {
    uint8_t index = code[offset + 1];
    printf("%02u: %-20s #%4u ", offset, get_opcode_name(opcode), index);
    print_constant_info_at(cf, index);
    printf("\n");
    return offset + 2;
}

// instructions that have one byte immediate value
int byte_immediate_inst(opcode_t opcode, uint8_t *code, uint32_t offset) {
    uint8_t byte = code[offset + 1];
    printf("%02u: %-20s #%4u ", offset, get_opcode_name(opcode), byte);
    printf("\n");
    return offset + 2;
}

// instructions that have two byte immediate value
int branch_offset_inst(opcode_t opcode, uint8_t *code, uint32_t offset) {
    //uint16_t word = (code[offset + 1] << 8) | (code[offset + 2]);
    int16_t word = (code[offset + 1] << 8) | (code[offset + 2]);
    printf("%02u: %-20s %4d ", offset, get_opcode_name(opcode), offset + word);
    printf("\n");
    return offset + 3;
}

// instructions that have two byte index
int word_index_inst(class_file_t *cf, opcode_t opcode, uint8_t *code, uint32_t offset) {
    uint16_t index = (code[offset + 1] << 8) | (code[offset + 2]);
    printf("%02u: %-20s #%4u ", offset, get_opcode_name(opcode), index);
    print_constant_info_at(cf, index);
    printf("\n");
    return offset + 3;
}


int disassemble_inst(class_file_t *cf, uint8_t *code, uint32_t offset, int indent_level) {
    uint8_t opcode = code[offset];
    printf("%*s", indent_level * 2, "");
    printf("(0x%02x) ", opcode);
    switch(opcode) {
        case OP_NOP: // 0x00
            return simple_inst(opcode, offset);
        case OP_ACONST_NULL: // 0x01
            return simple_inst(opcode, offset);
        case OP_ICONST_0: // 0x03
            return simple_inst(opcode, offset);
        case OP_ICONST_1: // 0x04
            return simple_inst(opcode, offset);
        case OP_ICONST_2:
            return simple_inst(opcode, offset);
        case OP_BIPUSH: // 0x10
            return byte_immediate_inst(opcode, code, offset);
        case OP_LDC: // 0x12
            return byte_index_inst(cf, opcode, code, offset);
        case OP_LLOAD: // 0x16
            return byte_index_inst(cf, opcode, code, offset);
        case OP_ALOAD_0:
            return simple_inst(opcode, offset);
        case OP_ISTORE_0:
            return simple_inst(opcode, offset);
        case OP_ISTORE_1:
            return simple_inst(opcode, offset);
        case OP_ISTORE_2:
            return simple_inst(opcode, offset);
        case OP_ISTORE_3:
            return simple_inst(opcode, offset);
        case OP_GETSTATIC:
            return word_index_inst(cf, opcode, code, offset);
        case OP_INVOKEVIRTUAL:
            return word_index_inst(cf, opcode, code, offset);
        case OP_INVOKESPECIAL:
            return word_index_inst(cf, opcode, code, offset);
        case OP_INVOKESTATIC:
            return word_index_inst(cf, opcode, code, offset);
        case OP_ILOAD_0:
            return simple_inst(opcode, offset);
        case OP_ILOAD_1:
            return simple_inst(opcode, offset);
        case OP_IINC: // 0x84
            return two_byte_value_inst(opcode, code, offset);
        case OP_IADD: // 0x60
            return simple_inst(opcode, offset);
        case OP_INEG: // 0x74
            return simple_inst(opcode, offset);
        case OP_IF_ICMPEQ: // 0x9f
            return branch_offset_inst(opcode, code, offset);
        case OP_IF_ICMPNE: // 0xa0
            return branch_offset_inst(opcode, code, offset);
        case OP_IF_ICMPLT: // 0xa1
            return branch_offset_inst(opcode, code, offset);
        case OP_IF_ICMPGE: // 0xa2
            return branch_offset_inst(opcode, code, offset);
        case OP_IF_ICMPGT: // 0xa3
            return branch_offset_inst(opcode, code, offset);
        case OP_IF_ICMPLE: // 0xa4
            return branch_offset_inst(opcode, code, offset);
        case OP_GOTO: // 0xa7
            return branch_offset_inst(opcode, code, offset);
        case OP_IRETURN:
            return simple_inst(opcode, offset);
        case OP_RETURN:
            return simple_inst(opcode, offset);
        default:
            printf("Unknown opcode %s (0x%02x)\n", get_opcode_name(opcode), opcode);
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
    dump_attributes(cf, code_attr->attributes_count, code_attr->attributes, indent_level + 1);
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

void dump_attr_line_num_table(attribute_t *attr, int indent_level) {
    printf("%*sLine Number Table :\n", indent_level * 2, "");
    //dump_attribute_common(cf, attr, indent_level + 1);
    attr_line_number_table_t *a = attr->info.attr_line_number_table;
    //printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "Line Number Table Length", a->line_number_table_length);
    for (uint16_t i = 0; i < a->line_number_table_length; i++) {
        printf("%*s%s %d: %u\n", (indent_level + 1) * 2, "",     "Line",
               a->line_number_table[i].line_number,
               a->line_number_table[i].start_pc);
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
                dump_attr_line_num_table(&attributes[i], indent_level + 1);
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
    dump_constant_pool(class_file, class_file->constant_pool_count, class_file->constant_pool);
    printf("%-25s 0x%04x '\n", "Access Flags", class_file->access_flags);
    printf("%-25s 0x%04x // %s'\n", "This class", class_file->this_class, get_classname(class_file, class_file->this_class));
    printf("%-25s 0x%04x '\n", "Super class", class_file->super_class);
    dump_interfaces(class_file->interfaces_count, class_file->interfaces);
    dump_fields(class_file->fields_count, class_file->fields);
    dump_methods(class_file, class_file->methods_count, class_file->methods);
    dump_attributes(class_file, class_file->attributes_count, class_file->attributes, 0);
}
