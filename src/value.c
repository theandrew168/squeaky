#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"

struct value*
value_make_number(double number)
{
    struct value* value = malloc(sizeof(value));
    value->type = VALUE_NUMBER;
    value->as.number = number;
    return value;
}

struct value*
value_make_string(const char* string, long length)
{
    assert(string != NULL);

    struct value* value = malloc(sizeof(value));
    value->type = VALUE_STRING;
    value->as.string = malloc(length -2 + 1);  // don't keep the two quote chars
    strncpy(value->as.string, string + 1, length - 2);
    return value;
}

struct value*
value_make_symbol(const char* symbol, long length)
{
    assert(symbol != NULL);

    struct value* value = malloc(sizeof(value));
    value->type = VALUE_SYMBOL;
    value->as.symbol = malloc(length + 1);
    strncpy(value->as.symbol, symbol, length);
    return value;
}

struct value*
value_make_pair(struct value* car, struct value* cdr)
{
    struct value* value = malloc(sizeof(value));
    value->type = VALUE_PAIR;
    value->as.pair.car = car;
    value->as.pair.cdr = cdr;
    return value;
}

void
value_free(struct value* value)
{
    assert(value != NULL);

    switch (value->type) {
        case VALUE_NUMBER: break;
        case VALUE_STRING: free(value->as.string); break;
        case VALUE_SYMBOL: free(value->as.symbol); break;
        case VALUE_PAIR:
            value_free(value->as.pair.car);
            value_free(value->as.pair.cdr);
            break;
    }

    free(value);
}

static bool
is_tagged_list(struct value* value, const char* tag)
{
    if (value->type != VALUE_PAIR) return false;
    if (CAR(value)->type != VALUE_SYMBOL) return false;

    return strcmp(CAR(value)->as.symbol, tag) == 0;
}

bool
value_is_self_evaluating(struct value* value)
{
    assert(value != NULL);

    return value->type == VALUE_NUMBER || value->type == VALUE_STRING;
}

bool
value_is_variable(struct value* value)
{
    assert(value != NULL);

    return value->type == VALUE_SYMBOL;
}

bool
value_is_quoted(struct value* value)
{
    assert(value != NULL);

    return is_tagged_list(value, "quote");
}

bool
value_is_assignment(struct value* value)
{
    assert(value != NULL);

    return is_tagged_list(value, "set!");
}

bool
value_is_definition(struct value* value)
{
    assert(value != NULL);

    return is_tagged_list(value, "define");
}

void
value_print(const struct value* value)
{
    assert(value != NULL);

    switch (value->type) {
        case VALUE_NUMBER: printf("%lf", value->as.number); break;
        case VALUE_STRING: printf("\"%s\"", value->as.string); break;
        case VALUE_SYMBOL: printf("%s", value->as.symbol); break;
        case VALUE_PAIR:
            if (CAR(value) == NULL && CDR(value) == NULL) {
                printf("()");
                return;
            }

            printf("( ");
            if (value->as.pair.car == NULL) printf("()");
            else value_print(value->as.pair.car);
            printf(" . ");
            if (value->as.pair.cdr == NULL) printf("()");
            else value_print(value->as.pair.cdr);
            printf(" )");
            break;
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
