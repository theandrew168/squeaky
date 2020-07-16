#include <assert.h>
#include <stdbool.h>
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
object_make_boolean(struct object* object, bool value)
{
    assert(object != NULL);

    object->type = OBJECT_TYPE_BOOLEAN;
    object->data.boolean.value = value;
}

void
object_make_character(struct object* object, char value)
{
    assert(object != NULL);

    object->type = OBJECT_TYPE_CHARACTER;
    object->data.character.value = value;
}

void
object_print(const struct object* object)
{
    assert(object != NULL);

    switch (object->type) {
        case OBJECT_TYPE_FIXNUM:
            printf("%ld", object->data.fixnum.value);
            break;
        case OBJECT_TYPE_BOOLEAN:
            printf("#%c", object->data.boolean.value ? 't' : 'f');
            break;
        case OBJECT_TYPE_CHARACTER: {
            char c = object->data.character.value;
            printf("#\\");
            switch (c) {
                case '\n': printf("newline"); break;
                case ' ': printf("space"); break;
                default: putchar(c);
            }
        }   break;
        default:
            fprintf(stderr, "cannot print unknown type\n");
            break;
    }
}
