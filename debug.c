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

static char *opcode_name_map[] = {
        [OP_NOP]              = "nop",
        [OP_ACONST_NULL]      = "aconst_null",
        [OP_ICONST_M1]        = "iconst_m1",
        [OP_ICONST_0]         = "iconst_0",
        [OP_ICONST_1]         = "iconst_1",
        [OP_ICONST_2]         = "iconst_2",
        [OP_ICONST_3]         = "iconst_3",
        [OP_ICONST_4]         = "iconst_4",
        [OP_ICONST_5]         = "iconst_5",
        [OP_LCONST_0]         = "lconst_0",
        [OP_LCONST_1]         = "lconst_1",
        [OP_FCONST_0]         = "fconst_0",
        [OP_FCONST_1]         = "fconst_1",
        [OP_FCONST_2]         = "fconst_2",
        [OP_DCONST_0]         = "dconst_0",
        [OP_DCONST_1]         = "dconst_1",
        [OP_BIPUSH]           = "bipush",
        [OP_SIPUSH]           = "sipush",
        [OP_LDC]              = "ldc",
        [OP_LDC_W]            = "ldc_w",
        [OP_LDC2_W]           = "ldc2_w",
        [OP_ILOAD]            = "iload",
        [OP_LLOAD]            = "lload",
        [OP_FLOAD]            = "fload",
        [OP_DLOAD]            = "dload",
        [OP_ALOAD]            = "aload",
        [OP_ILOAD_0]          = "iload_0",
        [OP_ILOAD_1]          = "iload_1",
        [OP_ILOAD_2]          = "iload_2",
        [OP_ILOAD_3]          = "iload_3",
        [OP_LLOAD_0]          = "lload_0",
        [OP_LLOAD_1]          = "lload_1",
        [OP_LLOAD_2]          = "lload_2",
        [OP_LLOAD_3]          = "lload_3",
        [OP_FLOAD_0]          = "fload_0",
        [OP_FLOAD_1]          = "fload_1",
        [OP_FLOAD_2]          = "fload_2",
        [OP_FLOAD_3]          = "fload_3",
        [OP_DLOAD_0]          = "dload_0",
        [OP_DLOAD_1]          = "dload_1",
        [OP_DLOAD_2]          = "dload_2",
        [OP_DLOAD_3]          = "dload_3",
        [OP_ALOAD_0]          = "aload_0",
        [OP_ALOAD_1]          = "aload_1",
        [OP_ALOAD_2]          = "aload_2",
        [OP_ALOAD_3]          = "aload_3",
        [OP_IALOAD]           = "iaload",
        [OP_LALOAD]           = "laload",
        [OP_FALOAD]           = "faload",
        [OP_DALOAD]           = "daload",
        [OP_AALOAD]           = "aaload",
        [OP_BALOAD]           = "baload",
        [OP_CALOAD]           = "caload",
        [OP_SALOAD]           = "saload",
        [OP_ISTORE]           = "istore",
        [OP_LSTORE]           = "lstore",
        [OP_FSTORE]           = "fstore",
        [OP_DSTORE]           = "dstore",
        [OP_ASTORE]           = "astore",
        [OP_ISTORE_0]         = "istore_0",
        [OP_ISTORE_1]         = "istore_1",
        [OP_ISTORE_2]         = "istore_2",
        [OP_ISTORE_3]         = "istore_3",
        [OP_LSTORE_0]         = "lstore_0",
        [OP_LSTORE_1]         = "lstore_1",
        [OP_LSTORE_2]         = "lstore_2",
        [OP_LSTORE_3]         = "lstore_3",
        [OP_FSTORE_0]         = "fstore_0",
        [OP_FSTORE_1]         = "fstore_1",
        [OP_FSTORE_2]         = "fstore_2",
        [OP_FSTORE_3]         = "fstore_3",
        [OP_DSTORE_0]         = "dstore_0",
        [OP_DSTORE_1]         = "dstore_1",
        [OP_DSTORE_2]         = "dstore_2",
        [OP_DSTORE_3]         = "dstore_3",
        [OP_ASTORE_0]         = "astore_0",
        [OP_ASTORE_1]         = "astore_1",
        [OP_ASTORE_2]         = "astore_2",
        [OP_ASTORE_3]         = "astore_3",
        [OP_IASTORE]          = "iastore",
        [OP_LASTORE]          = "lastore",
        [OP_FASTORE]          = "fastore",
        [OP_DASTORE]          = "dastore",
        [OP_AASTORE]          = "aastore",
        [OP_BASTORE]          = "bastore",
        [OP_CASTORE]          = "castore",
        [OP_SASTORE]          = "sastore",
        [OP_POP]              = "pop",
        [OP_POP2]             = "pop2",
        [OP_DUP]              = "dup",
        [OP_DUP_X1]           = "dup_x1",
        [OP_DUP_X2]           = "dup_x2",
        [OP_DUP2]             = "dup2",
        [OP_DUP2_X1]          = "dup2_x1",
        [OP_DUP2_X2]          = "dup2_x2",
        [OP_SWAP]             = "swap",
        [OP_IADD]             = "iadd",
        [OP_LADD]             = "ladd",
        [OP_FADD]             = "fadd",
        [OP_DADD]             = "dadd",
        [OP_ISUB]             = "isub",
        [OP_LSUB]             = "lsub",
        [OP_FSUB]             = "fsub",
        [OP_DSUB]             = "dsub",
        [OP_IMUL]             = "imul",
        [OP_LMUL]             = "lmul",
        [OP_FMUL]             = "fmul",
        [OP_DMUL]             = "dmul",
        [OP_IDIV]             = "idiv",
        [OP_LDIV]             = "ldiv",
        [OP_FDIV]             = "fdiv",
        [OP_DDIV]             = "ddiv",
        [OP_IREM]             = "irem",
        [OP_LREM]             = "lrem",
        [OP_FREM]             = "frem",
        [OP_DREM]             = "drem",
        [OP_INEG]             = "ineg",
        [OP_LNEG]             = "lneg",
        [OP_FNEG]             = "fneg",
        [OP_DNEG]             = "dneg",
        [OP_ISHL]             = "ishl",
        [OP_LSHL]             = "lshl",
        [OP_ISHR]             = "ishr",
        [OP_LSHR]             = "lshr",
        [OP_IUSHR]            = "iushr",
        [OP_LUSHR]            = "lushr",
        [OP_IAND]             = "iand",
        [OP_LAND]             = "land",
        [OP_IOR]              = "ior",
        [OP_LOR]              = "lor",
        [OP_IXOR]             = "ixor",
        [OP_LXOR]             = "lxor",
        [OP_IINC]             = "iinc",
        [OP_I2L]              = "i2l",
        [OP_I2F]              = "i2f",
        [OP_I2D]              = "i2d",
        [OP_L2I]              = "l2i",
        [OP_L2F]              = "l2f",
        [OP_L2D]              = "l2d",
        [OP_F2I]              = "f2i",
        [OP_F2L]              = "f2l",
        [OP_F2D]              = "f2d",
        [OP_D2I]              = "d2i",
        [OP_D2L]              = "d2l",
        [OP_D2F]              = "d2f",
        [OP_I2B]              = "i2b",
        [OP_I2C]              = "i2c",
        [OP_I2S]              = "i2s",
        [OP_LCMP]             = "lcmp",
        [OP_FCMPL]            = "fcmpl",
        [OP_FCMPG]            = "fcmpg",
        [OP_DCMPL]            = "dcmpl",
        [OP_DCMPG]            = "dcmpg",
        [OP_IFEQ]             = "ifeq",
        [OP_IFNE]             = "ifne",
        [OP_IFLT]             = "iflt",
        [OP_IFGE]             = "ifge",
        [OP_IFGT]             = "ifgt",
        [OP_IFLE]             = "ifle",
        [OP_IF_ICMPEQ]        = "if_icmpeq",
        [OP_IF_ICMPNE]        = "if_icmpne",
        [OP_IF_ICMPLT]        = "if_icmplt",
        [OP_IF_ICMPGE]        = "if_icmpge",
        [OP_IF_ICMPGT]        = "if_icmpgt",
        [OP_IF_ICMPLE]        = "if_icmple",
        [OP_IF_ACMPEQ]        = "if_acmpeq",
        [OP_IF_ACMPNE]        = "if_acmpne",
        [OP_GOTO]             = "goto",
        [OP_JSR]              = "jsr",
        [OP_RET]              = "ret",
        [OP_TABLESWITCH]      = "tableswitch",
        [OP_LOOKUPSWITCH]     = "lookupswitch",
        [OP_IRETURN]          = "ireturn",
        [OP_LRETURN]          = "lreturn",
        [OP_FRETURN]          = "freturn",
        [OP_DRETURN]          = "dreturn",
        [OP_ARETURN]          = "areturn",
        [OP_RETURN]           = "return",
        [OP_GETSTATIC]        = "getstatic",
        [OP_PUTSTATIC]        = "putstatic",
        [OP_GETFIELD]         = "getfield",
        [OP_PUTFIELD]         = "putfield",
        [OP_INVOKEVIRTUAL]    = "invokevirtual",
        [OP_INVOKESPECIAL]    = "invokespecial",
        [OP_INVOKESTATIC]     = "invokestatic",
        [OP_INVOKEINTERFACE]  = "invokeinterface",
        [OP_INVOKEDYNAMIC]    = "invokedynamic",
        [OP_NEW]              = "new",
        [OP_NEWARRAY]         = "newarray",
        [OP_ANEWARRAY]        = "anewarray",
        [OP_ARRAYLENGTH]      = "arraylength",
        [OP_ATHROW]           = "athrow",
        [OP_CHECKCAST]        = "checkcast",
        [OP_INSTANCEOF]       = "instanceof",
        [OP_MONITORENTER]     = "monitorenter",
        [OP_MONITOREXIT]      = "monitorexit",
        [OP_WIDE]             = "wide",
        [OP_MULTIANEWARRAY]   = "multianewarray",
        [OP_IFNULL]           = "ifnull",
        [OP_IFNONNULL]        = "ifnonnull",
        [OP_GOTO_W]           = "goto_w",
        [OP_JSR_W]            = "jsr_w",
        [OP_BREAKPOINT]       = "breakpoint",
        [OP_IMPDEP1]          = "impdep1",
        [OP_IMPDEP2]          = "impdep2"
};

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
        case OP_ACONST_NULL: // 0x01
        case OP_ICONST_0: // 0x03
        case OP_ICONST_1: // 0x04
        case OP_ICONST_2:
            return simple_inst(opcode, offset);
        case OP_BIPUSH: // 0x10
            return byte_immediate_inst(opcode, code, offset);
        case OP_LDC: // 0x12
        case OP_LLOAD: // 0x16
            return byte_index_inst(cf, opcode, code, offset);
        case OP_ALOAD_0:
        case OP_ISTORE_0:
        case OP_ISTORE_1:
        case OP_ISTORE_2:
        case OP_ISTORE_3:
            return simple_inst(opcode, offset);
        case OP_GETSTATIC:
        case OP_INVOKEVIRTUAL:
        case OP_INVOKESPECIAL:
        case OP_INVOKESTATIC:
            return word_index_inst(cf, opcode, code, offset);
        case OP_ILOAD_0:
        case OP_ILOAD_1:
            return simple_inst(opcode, offset);
        case OP_IINC: // 0x84
            return two_byte_value_inst(opcode, code, offset);
        case OP_IADD: // 0x60
        case OP_ISUB: // 0x64
        case OP_INEG: // 0x74
            return simple_inst(opcode, offset);
        case OP_IFEQ: // 0x99
        case OP_IFNE: // 0x9a
        case OP_IFLT: // 0x9b
        case OP_IFGE: // 0x9c
        case OP_IFGT: // 0x9d
        case OP_IFLE: // 0x9e
        case OP_IF_ICMPEQ: // 0x9f
        case OP_IF_ICMPNE: // 0xa0
        case OP_IF_ICMPLT: // 0xa1
        case OP_IF_ICMPGE: // 0xa2
        case OP_IF_ICMPGT: // 0xa3
        case OP_IF_ICMPLE: // 0xa4
        case OP_GOTO: // 0xa7
            return branch_offset_inst(opcode, code, offset);
        case OP_IRETURN:
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

void dump_attr_nestmembers(class_file_t *cf, attribute_t *attr, int indent_level) {
    attr_nestmembers_t *nestmembers = attr->info.attr_nestmembers;
    printf("%*sNestmembers Attribute :\n", (indent_level + 0) * 2, "");
    dump_attribute_common(cf, attr, indent_level + 1);
    for (int i = 0; i < nestmembers->number_of_classes; i++) {
        uint16_t index = nestmembers->classes[i];
        print_constant_info_at(cf, index);
    }
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
            case ATTR_NEST_MEMBERS:
                dump_attr_nestmembers(class_file, &attributes[i], indent_level + 1);
                break;
            case ATTR_UNKNOWN:
            default:
                printf("%*s%-20s = 0x%04x\n", (indent_level + 1) * 2, "", "Unknown Attribute", attributes[i].tag);
                break;
        }
    }
}


static char *access_flags_map[] = {
        [ACC_PUBLIC]     = "ACC_PUBLIC",
        [ACC_PRIVATE]    = "ACC_PRIVATE",
        [ACC_FINAL]      = "ACC_FINAL",
        [ACC_SUPER]      = "ACC_SUPER",
        [ACC_INTERFACE]  = "ACC_INTERFACE",
        [ACC_ABSTRACT]   = "ACC_ABSTRACT",
        [ACC_SYNTHETIC]  = "ACC_SYNTHETIC",
        [ACC_ANNOTATION] = "ACC_ANNOTATION",
        [ACC_ENUM]       = "ACC_ENUM",
        [ACC_MODULE]     = "ACC_MODULE",
};

char *get_access_flag_str(access_flags_t flag) {
    return access_flags_map[flag];
}

void dump_access_flags(uint16_t access_flags) {
    printf("    %-20s = (0x%04x) ", "Access Flags", access_flags);
    if (access_flags & ACC_PUBLIC)    { printf("ACC_PUBLIC, "); }
    if (access_flags & ACC_PRIVATE)   { printf("ACC_PRIVATE, "); }
    if (access_flags & ACC_PROTECTED) { printf("ACC_PROTECTED, "); }
    if (access_flags & ACC_STATIC)    { printf("ACC_STATIC, "); }
    if (access_flags & ACC_FINAL)     { printf("ACC_FINAL, "); }
    printf("\n");
}

void dump_methods(class_file_t *class_file, uint16_t count, method_t *methods) {
    //class_file_t *class_file;
    printf("Methods : %u\n", count);
    for (uint16_t i = 0; i < count; i++) {
        printf("  Method[%u]:\n", i);
        dump_access_flags(methods[i].access_flags);
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
