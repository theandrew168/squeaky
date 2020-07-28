#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"

struct value*
value_make_boolean(bool boolean)
{
    struct value* value = malloc(sizeof(value));
    value->type = VALUE_BOOLEAN;
    value->as.boolean = boolean;
    return value;
}

struct value*
value_make_number(double number)
{
    struct value* value = malloc(sizeof(value));
    value->type = VALUE_NUMBER;
    value->as.number = number;
    return value;
}

struct value*
value_make_character(char character)
{
    struct value* value = malloc(sizeof(value));
    value->type = VALUE_CHARACTER;
    value->as.character = character;
    return value;
}

struct value*
value_make_string(const char* string)
{
    assert(string != NULL);

    struct value* value = malloc(sizeof(value));
    value->type = VALUE_STRING;
    value->as.string = malloc(strlen(string) + 1);
    strcpy(value->as.string, string);
    return value;
}

struct value*
value_make_symbol(const char* symbol)
{
    assert(symbol != NULL);

    struct value* value = malloc(sizeof(value));
    value->type = VALUE_SYMBOL;
    value->as.symbol = malloc(strlen(symbol) + 1);
    strcpy(value->as.symbol, symbol);
    return value;
}

void
value_free(struct value* value)
{
    assert(value != NULL);

    switch (value->type) {
        case VALUE_BOOLEAN: break;
        case VALUE_NUMBER: break;
        case VALUE_CHARACTER: break;
        case VALUE_STRING: free(value->as.string); break;
        case VALUE_SYMBOL: free(value->as.symbol); break;
    }

    free(value);
}

void
value_print(const struct value* value)
{
    assert(value != NULL);

    switch (value->type) {
        case VALUE_BOOLEAN: printf("%s", value->as.boolean ? "#t" : "#f"); break;
        case VALUE_NUMBER: printf("%lf", value->as.number); break;
        case VALUE_CHARACTER: printf("#\\%c", value->as.character); break;
        case VALUE_STRING: printf("\"%s\"", value->as.string); break;
        case VALUE_SYMBOL: printf("%s", value->as.symbol); break;
        default:
            printf("<undefined>");
    }
}

void
value_println(const struct value* value)
{
    assert(value != NULL);

    value_print(value);
    printf("\n");
}

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
value_array_append(struct value_array* array, struct value* value)
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
        array->values = realloc(array->values, sizeof(struct value*) * array->capacity);
        assert(array->values != NULL);
    }

    // now that the buffer is big enough, append the new value and inc the count
    array->values[array->count] = value;
    array->count++;
}
