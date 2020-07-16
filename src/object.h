#ifndef SQUEAKY_OBJECT_H_INCLUDED
#define SQUEAKY_OBJECT_H_INCLUDED

#include <stdbool.h>

enum object_type {
    OBJECT_TYPE_UNDEFINED = 0,
    OBJECT_TYPE_FIXNUM,
};

struct object {
    int type;
    union {
        struct {
            long value;
        } fixnum;
    } data;
};

void object_make_fixnum(struct object* object, long value);
void object_print(const struct object* object);

#endif
