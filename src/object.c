#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "object.h"

void
object_make_fixnum(struct object* object, long value)
{
    assert(object != NULL);

    object->type = OBJECT_TYPE_FIXNUM;
    object->data.fixnum.value = value;
}

void
object_print(const struct object* object)
{
    assert(object != NULL);

    switch (object->type) {
        case OBJECT_TYPE_FIXNUM:
            printf("%ld", object->data.fixnum.value);
            break;
        default:
            fprintf(stderr, "cannot print unknown type\n");
            break;
    }
}
