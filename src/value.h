#ifndef SQUEAKY_VALUE_H_INCLUDED
#define SQUEAKY_VALUE_H_INCLUDED

#include <stdbool.h>

enum value_type {
    VALUE_UNDEFINED = 0,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_SYMBOL,
    VALUE_PAIR,
};

struct value {
    int type;
    union {
        double number;
        char* string;
        char* symbol;
        struct {
            struct value* left;
            struct value* right;
        } pair;
    } as;
};

struct value* value_make_number(double number);
struct value* value_make_string(const char* string, long length);
struct value* value_make_symbol(const char* symbol, long length);
struct value* value_make_pair(struct value* left, struct value* right);
void value_free(struct value* value);

void value_print(const struct value* value);
void value_println(const struct value* value);

#endif
