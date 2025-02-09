#include "loader.h"
#include <stdio.h>
#include "memory.h"


char * get_constant_utf8(class_file_t *cf, uint16_t index) {
    constant_tag_t tag = cf->constant_pool[index - 1].tag;
    assert(tag == CONSTANT_UTF8 && "Constant is not Utf8");
    return cf->constant_pool[index - 1].info.utf8_info.bytes;
}

char * get_classname(class_file_t *cf, uint16_t index) {
    constant_class_info_t class_info = cf->constant_pool[index - 1].info.class_info;
    uint16_t name_index = class_info.name_index;
    return get_constant_utf8(cf, name_index);
}

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

static uint16_t read_word() {
    uint16_t out;
    read_bytes(&out, 2);
    return out;
}

void read_constant_pool(uint16_t count, constant_pool_t **constant_pool) {
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
    //current += count * sizeof(constant_pool_t);
}

// compare attribute name string to get attribute_tag_t

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

void read_attributes(class_file_t *cf, uint16_t count, attribute_t **attributes) {
    if (count == 0) {
        return;
    }
    attribute_t *a = ALLOCATE(attribute_t, count);
    for (uint16_t i = 0; i < count; i++) {
        uint16_t name_index = read_word();
        a[i].name_index = name_index;
        // lookup name in constant pool and map to tag
        char *name = get_constant_utf8(cf, name_index);
        attribute_tag_t tag = get_attribute_tag(name);
//        switch(tag) {
//            case ATTR_CODE:
//                break;
//            case ATTR_CODE:
//                break;
//        }
        // FIXME - current spot

        read_bytes(&a[i].attribute_length, 4);
       // copy info byte array
       // a[i].in
        a[i].info = ALLOCATE(uint8_t, a[i].attribute_length);
        memcpy(a[i].info, current, a[i].attribute_length);
        current += a[i].attribute_length;
    }
    *attributes = a;
}

void read_interfaces(uint16_t count, interface_t *interfaces) {
    if (count == 0) {
        return;
    }
    printf("TODO: read_interfaces\n");
    exit(-1);
}

void read_fields(uint16_t count, field_t *fields) {
    if (count == 0) {
        return;
    }
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
        read_bytes(&m[i].descriptor_index, 2);
        read_bytes(&m[i].attribute_count, 2);
        read_attributes(cf, m[i].attribute_count, &m[i].attributes);
    }
    *methods = m;
}

void read_class_file(char *bytes, class_file_t *class_file) {
    current = bytes;
    read_bytes(&class_file->magic, 4);
    if (class_file->magic != 0xCAFEBABE) {
        printf("Magic Number error\n");
        exit(EXIT_MAGIC_ERROR);
    }
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
    read_methods(class_file, class_file->methods_count, &class_file->methods);
    read_bytes(&class_file->attributes_count, 2);
    read_attributes(class_file, class_file->attributes_count, &class_file->attributes);
    // FIXME - probably a good idea to check we're at end of buffer
}