#include <stdio.h>
#include <stdarg.h>
#include "memory.h"
#include <string.h>
#include "debug.h"

#ifdef DEBUG
#define debug_print(fmt, ...) \
            do { fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#else
#define debug_print(fmt, ...) do { ; } while (0)
#endif

typedef enum parse_result_t {
    PARSE_OK,
    PARSE_ERROR
} parse_result_t;

void runtime_error(const char * format, ...);

void parse_error(class_file_t *cf, const char * format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    dump_classfile(cf);

    exit(-1);
}

char * get_constant_utf8(class_file_t *cf, uint16_t index) {
    constant_tag_t tag = cf->constant_pool[index - 1].tag;
    assert(tag == CONSTANT_UTF8 && "Constant is not Utf8");
    return cf->constant_pool[index - 1].info.utf8_info.bytes;
}

// get class name for constant class info at index
char * get_classname(class_file_t *cf, uint16_t index) {
    constant_tag_t tag = cf->constant_pool[index - 1].tag;
    constant_class_info_t class_info = cf->constant_pool[index - 1].info.class_info;
    assert(tag == CONSTANT_CLASS && "Constant is not Class");
    uint16_t name_index = class_info.name_index;
    return get_constant_utf8(cf, name_index);
}

method_t *get_class_method(class_file_t *class, const char *name, const char* descriptor) {
    for (int i = 0; i < class->methods_count; i++) {
        if (strcmp(name,       get_constant_utf8(class, class->methods[i].name_index)) == 0 &&
            strcmp(descriptor, get_constant_utf8(class, class->methods[i].descriptor_index)) == 0) {
            return &class->methods[i];
        }
    }
    return NULL;
}


// read unsigned integer into dst

uint8_t *current;

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

static uint8_t read_byte(void) {
    uint8_t out;
    read_bytes(&out, 1);
    return out;
}

//static uint16_t read_word() {
//    uint16_t out;
//    read_bytes(&out, 2);
//    return out;
//}

static char *constant_tag_map[] = {
        [CONSTANT_CLASS] = "Class",
        [CONSTANT_FIELDREF] = "Fieldref",
        [CONSTANT_METHOD_REF] = "Methodref",
        [CONSTANT_INTERFACE_METHOD_REF] = "InterfaceMethodref",
        [CONSTANT_STRING] = "String",
        [CONSTANT_INTEGER] = "Integer",
        [CONSTANT_FLOAT] = "Float",
        [CONSTANT_LONG] = "Long",
        [CONSTANT_DOUBLE] = "Double",
        [CONSTANT_NAME_AND_TYPE] = "NameAndType",
        [CONSTANT_UTF8] = "Utf8",
        [CONSTANT_METHOD_HANDLE] = "MethodHandle",
        [CONSTANT_METHOD_TYPE] = "MethodType",
        [CONSTANT_UNKNOWN] = "*Unknown*",
};

char *get_constant_tag_name(constant_tag_t tag) {
    return constant_tag_map[tag];
}

void read_constant_pool(class_file_t *cf, uint16_t count, constant_pool_t **constant_pool) {
    constant_pool_t *cp = ALLOCATE(constant_pool_t, count);
    // constant pool count is number of entries PLUS ONE
    for (uint16_t i = 0; i < count - 1; i++) {
        uint8_t tag = read_byte();
        cp[i].tag = tag;
        switch(tag) {
            case CONSTANT_CLASS:
                read_bytes(&cp[i].info.class_info.name_index, 2);
                break;
            case CONSTANT_FIELDREF:
                read_bytes(&cp[i].info.field_ref_info.class_index, 2);
                read_bytes(&cp[i].info.field_ref_info.name_and_type_index, 2);
                break;
            case CONSTANT_METHOD_REF:
                read_bytes(&cp[i].info.method_ref_info.class_index, 2);
                read_bytes(&cp[i].info.method_ref_info.name_and_type_index, 2);
                break;
            case CONSTANT_STRING:
                read_bytes(&cp[i].info.string_info.string_index, 2);
                break;
            case CONSTANT_NAME_AND_TYPE:
                read_bytes(&cp[i].info.name_and_type_info.name_index, 2);
                read_bytes(&cp[i].info.name_and_type_info.descriptor_index, 2);
                break;
            case CONSTANT_UTF8:
                read_bytes(&cp[i].info.utf8_info.length, 2);
                cp[i].info.utf8_info.bytes = copy_string((char*) current, cp[i].info.utf8_info.length);
                current += cp[i].info.utf8_info.length;
                break;
            default:
                parse_error(cf, "Error: Unknown tag 0x%02d for constant pool entry %d (%s)", tag, i + 1, get_constant_tag_name(tag));
        }
    }
    *constant_pool = cp;
    //current += count * sizeof(constant_pool_t);
}

// compare attribute name string to get attribute_tag_t

static char *attribute_tag_map[] = {
        [ATTR_CODE] = "Code",
        [ATTR_CONSTANT_VALUE] = "ConstantValue",
        [ATTR_STACK_MAP_TABLE] = "StackMapTable",
        [ATTR_BOOTSTRAP_METHODS] = "BootstrapMethods",
        [ATTR_NEST_HOST] = "NestHost",
        [ATTR_NEST_MEMBERS] = "NestMembers",
        [ATTR_SOURCE_FILE] = "SourceFile",
        [ATTR_LINE_NUM_TABLE] = "LineNumberTable",
        [ATTR_UNKNOWN] = "*Unknown*"
};

char *get_attribute_name(attribute_tag_t tag) {
    return attribute_tag_map[tag];
}

attribute_tag_t get_attribute_tag(char *str) {
    for (uint8_t i = 0; i <= ATTR_UNKNOWN; i++) {
        if (strcmp(str, attribute_tag_map[i]) == 0) {
            return i;
        }
    }
    return ATTR_UNKNOWN;
}

void read_attributes(class_file_t *cf, uint16_t count, attribute_t **attributes);

attribute_t *get_attribute_by_tag(int16_t attributes_count, attribute_t *attributes, attribute_tag_t tag) {
    for (int16_t i = 0; i < attributes_count; i++) {
        if (attributes[i].tag == tag) {
            return &attributes[i];
        }
    }
    return NULL;
}


constant_pool_t *get_constant(class_file_t *cf, uint16_t index) {
    return &cf->constant_pool[index - 1];
}

// return entry from constant pool and verify it has expected tag
constant_pool_t *get_constant_exp(class_file_t *cf, uint16_t index, constant_tag_t expected) {
    constant_tag_t actual = cf->constant_pool[index - 1].tag;
    if (expected != CONSTANT_UNKNOWN && actual != expected) {
        runtime_error("get_constant_exp index = %d. Expected tag %s but found %s\n",
                      get_constant_tag_name(expected),
                      get_constant_tag_name(actual));
    }
    return &cf->constant_pool[index - 1];
}

// parsing this sounds really annoying so im skipping it for now
attr_stack_map_table_t *read_attribute_stack_map_table(class_file_t *cf, uint32_t size) {
    (void) cf;
    attr_stack_map_table_t *a = ALLOCATE(attr_stack_map_table_t, 1);
    read_bytes(&a->number_of_entries, 2);
    a->entries = ALLOCATE(uint8_t, size - 2);
    memcpy(a->entries, current, size - 2);
    current += size - 2;
    return a;
}

attr_code_t *read_attribute_code(class_file_t *cf) {
    attr_code_t *a = ALLOCATE(attr_code_t, 1);
    read_bytes(&a->max_stack, 2);
    read_bytes(&a->max_locals, 2);
    read_bytes(&a->code_length, 4);
    a->code = ALLOCATE(uint8_t, a->code_length);
    memcpy(a->code, current, a->code_length);
    current += a->code_length;
    read_bytes(&a->exception_table_length, 2);
    a->exception_table = ALLOCATE(exception_table_t , a->exception_table_length);
    for (uint16_t i = 0; i < a->exception_table_length; i++) {
        read_bytes(&a->exception_table[i].start_pc, 2);
        read_bytes(&a->exception_table[i].end_pc, 2);
        read_bytes(&a->exception_table[i].handler_pc, 2);
        read_bytes(&a->exception_table[i].catch_type, 2);
    }
    read_bytes(&a->attributes_count, 2);
    read_attributes(cf, a->attributes_count, &a->attributes);
    return a;
}

attr_line_number_table_t *read_attribute_line_number_table(void) {
    attr_line_number_table_t *a = ALLOCATE(attr_line_number_table_t, 1);
    read_bytes(&a->line_number_table_length, 2);
    a->line_number_table = ALLOCATE(line_number_table_t , a->line_number_table_length);
    for (uint16_t i = 0; i < a->line_number_table_length; i++) {
        read_bytes(&a->line_number_table[i].start_pc, 2);
        read_bytes(&a->line_number_table[i].line_number, 2);
    }
    return a;
}

attr_source_file_t *read_attribute_source_file(void) {
    attr_source_file_t *a = ALLOCATE(attr_source_file_t, 1);
    read_bytes(&a->sourcefile_index, 2);
    return a;
}

void read_attributes(class_file_t *cf, uint16_t count, attribute_t **attributes) {
    if (count == 0) {
        return;
    }
    attribute_t *a = ALLOCATE(attribute_t, count);
    for (uint16_t i = 0; i < count; i++) {
        read_bytes(&a[i].name_index, 2);
        debug_print("Processing Attribute %s\n", get_constant_utf8(cf, a[i].name_index));

        // lookup name in constant pool and map to tag
        char *name = get_constant_utf8(cf, a[i].name_index);
        a->tag = get_attribute_tag(name);

        read_bytes(&a[i].attribute_length, 4);
        switch (a->tag) {
            case ATTR_CODE:
                a[i].info.attr_code = read_attribute_code(cf);
                break;
            case ATTR_LINE_NUM_TABLE:
                a[i].info.attr_line_number_table = read_attribute_line_number_table();
                break;
            case ATTR_SOURCE_FILE:
                a[i].info.attr_source_file = read_attribute_source_file();
                break;
            case ATTR_STACK_MAP_TABLE:
                a[i].info.attr_stack_map_table = read_attribute_stack_map_table(cf, a[i].attribute_length);
                break;
            case ATTR_UNKNOWN:
            default:
                parse_error(cf, "Error: Unknown attribute tag 0x%02d for constant pool entry %d (%s)", a->tag, i + 1, get_attribute_name(a->tag));
        }
       // copy info byte array
       // a[i].in
        // a[i].info = ALLOCATE(uint8_t, a[i].attribute_length);
        // memcpy(a[i].info, current, a[i].attribute_length);
        // current += a[i].attribute_length;
    }
    *attributes = a;
}

method_t *get_methodref(class_file_t *cf, uint16_t index) {
    constant_method_ref_info_t method_ref_info = get_constant_exp(cf, index, CONSTANT_METHOD_REF)->info.method_ref_info;
    // lookup class & NameAndType in constant pool
    //constant_class_info_t class = get_constant_exp(cf, method_ref_info.class_index, CONSTANT_CLASS);
    constant_name_and_type_info_t name_and_type = get_constant_exp(
            cf, method_ref_info.name_and_type_index, CONSTANT_NAME_AND_TYPE
    )->info.name_and_type_info;
    char *method_name = get_constant_utf8(cf, name_and_type.name_index);
    char *method_desc = get_constant_utf8(cf, name_and_type.descriptor_index);
    return get_class_method(cf, method_name, method_desc);
}

void read_interfaces(uint16_t count, interface_t *interfaces) {
    if (count == 0) {
        return;
    }
    (void) interfaces; // suppress -Werror=unused-parameter
    printf("TODO: read_interfaces\n");
    exit(-1);
}

void read_fields(uint16_t count, field_t *fields) {
    if (count == 0) {
        return;
    }
    (void) fields; // suppress -Werror=unused-parameter
    printf("TODO: read_fields\n");
    exit(-1);
}

void read_methods(class_file_t *cf, uint16_t count, method_t **methods) {
    if (count == 0) {
        return;
    }
    method_t *m = ALLOCATE(method_t, count);
    for (uint16_t i = 0; i < count; i++) {
        read_bytes(&m[i].access_flags, 2);
        read_bytes(&m[i].name_index, 2);
        debug_print("Processing Method %s\n", get_constant_utf8(cf, m[i].name_index));
        read_bytes(&m[i].descriptor_index, 2);
        read_bytes(&m[i].attribute_count, 2);
        read_attributes(cf, m[i].attribute_count, &m[i].attributes);
    }
    *methods = m;
}

void read_class_file(uint8_t *bytes, class_file_t *class_file) {
    current = bytes;
    read_bytes(&class_file->magic, 4);
    if (class_file->magic != 0xCAFEBABE) {
        printf("Magic Number error\n");
        exit(EXIT_MAGIC_ERROR);
    }
    read_bytes(&class_file->minor_version, 2);
    read_bytes(&class_file->major_version, 2);
    read_bytes(&class_file->constant_pool_count, 2);
    read_constant_pool(class_file, class_file->constant_pool_count, &class_file->constant_pool);
    read_bytes(&class_file->access_flags, 2);
    read_bytes(&class_file->this_class, 2);
    read_bytes(&class_file->super_class, 2);
    read_bytes(&class_file->interfaces_count, 2);
    read_interfaces(class_file->interfaces_count, class_file->interfaces);
    read_bytes(&class_file->fields_count, 2);
    read_fields(class_file->fields_count, class_file->fields);
    read_bytes(&class_file->methods_count, 2);
    read_methods(class_file, class_file->methods_count, &class_file->methods);
    read_bytes(&class_file->attributes_count, 2);
    read_attributes(class_file, class_file->attributes_count, &class_file->attributes);
    // FIXME - probably a good idea to check we're at end of buffer
}
