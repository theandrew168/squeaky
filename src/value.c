#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "value.h"

void
value_array_init(struct value_array* array)
{
    assert(array != NULL);

    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void
value_array_free(struct value_array* array)
{
    assert(array != NULL);

    free(array->values);
    value_array_init(array);
}

void
value_array_append(struct value_array* array, Value value)
{
    assert(array != NULL);

    // if new count will exceed the current capacity, grow the buffer
    if (array->count + 1 > array->capacity) {
        if (array->capacity == 0) {
            // special case for first growth from zero to 8 (just a reasonable default)
            array->capacity = 8;
        } else {
            // else double the buffer's capacity
            array->capacity *= 2;
        }

        // realloc the buffer to the new capacity
        array->values = realloc(array->values, sizeof(Value) * array->capacity);
        assert(array->values != NULL);
    }

    // now that the buffer is big enough, append the new value and inc the count
    array->values[array->count] = value;
    array->count++;
}

void
value_print(Value value)
{
    printf("%g", value);
}
