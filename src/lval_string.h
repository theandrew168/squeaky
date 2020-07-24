#ifndef SQUEAKY_LVAL_STRING_H_INCLUDED
#define SQUEAKY_LVAL_STRING_H_INCLUDED

#include <stdbool.h>

union lval;

struct lval_string {
    int type;
    char* string;
};

#define AS_STRING(val) ((struct lval_string*)(val))
#define AS_CONST_STRING(val) ((const struct lval_string*)(val))

bool lval_string_init(union lval* val, const char* string);
void lval_string_free(union lval* val);
void lval_string_copy(const union lval* val, union lval* copy);

void lval_string_print(const union lval* val);
bool lval_string_equal(const union lval* a, const union lval* b);

#endif
