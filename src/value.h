#ifndef SQUEAKY_VALUE_H_INCLUDED
#define SQUEAKY_VALUE_H_INCLUDED

#include <stdbool.h>

enum value_type {
    VALUE_UNDEFINED = 0,
    VALUE_BOOLEAN,
    VALUE_NUMBER,
    VALUE_CHARACTER,
    VALUE_STRING,
    VALUE_SYMBOL,
};

struct value {
    int type;
    union {
        bool boolean;
        double number;
        char character;
        char* string;
        char* symbol;
    } as;
};

struct value* value_make_boolean(bool boolean);
struct value* value_make_number(double number);
struct value* value_make_character(char character);
struct value* value_make_string(const char* string);
struct value* value_make_symbol(const char* symbol);
void value_free(struct value* value);

void value_print(const struct value* value);
void value_println(const struct value* value);

struct value_array {
    long count;
    long capacity;
    struct value** values;
};

void value_array_init(struct value_array* array);
void value_array_free(struct value_array* array);
void value_array_append(struct value_array* array, struct value* value);

#endif
