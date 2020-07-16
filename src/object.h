#ifndef SQUEAKY_OBJECT_H_INCLUDED
#define SQUEAKY_OBJECT_H_INCLUDED

#include <stdbool.h>

enum object_type {
    OBJECT_TYPE_UNDEFINED = 0,
    OBJECT_TYPE_FIXNUM,
    OBJECT_TYPE_BOOLEAN,
    OBJECT_TYPE_CHARACTER,
};

struct object {
    int type;
    union {
        struct {
            long value;
        } fixnum;
        struct {
            bool value;
        } boolean;
        struct {
            char value;
        } character;
    } data;
};

void object_make_fixnum(struct object* object, long value);
void object_make_boolean(struct object* object, bool value);
void object_make_character(struct object* object, char value);
void object_print(const struct object* object);

#endif
