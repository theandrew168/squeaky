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
value_make_pair(struct value* left, struct value* right)
{
    struct value* value = malloc(sizeof(value));
    value->type = VALUE_PAIR;
    value->as.pair.left = left;
    value->as.pair.right = right;
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
            value_free(value->as.pair.left);
            value_free(value->as.pair.right);
            break;
    }

    free(value);
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
value_is_definition(struct value* value)
{
    assert(value != NULL);

    return value->type == VALUE_PAIR &&
           value->as.pair.left->type == VALUE_SYMBOL &&
           strcmp(value->as.pair.left->as.symbol, "define") == 0;
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
            printf("( ");
            if (value->as.pair.left == NULL) printf("()");
            else value_print(value->as.pair.left);
            printf(" . ");
            if (value->as.pair.right == NULL) printf("()");
            else value_print(value->as.pair.right);
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
