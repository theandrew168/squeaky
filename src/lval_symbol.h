#ifndef SQUEAKY_LVAL_SYMBOL_H_INCLUDED
#define SQUEAKY_LVAL_SYMBOL_H_INCLUDED

#include <stdbool.h>

union lval;

struct lval_symbol {
    int type;
    char* symbol;
};

#define AS_SYMBOL(val) ((struct lval_symbol*)(val))
#define AS_CONST_SYMBOL(val) ((const struct lval_symbol*)(val))

bool lval_symbol_init(union lval* val, const char* symbol);
void lval_symbol_free(union lval* val);
void lval_symbol_copy(const union lval* val, union lval* copy);

void lval_symbol_print(const union lval* val);
bool lval_symbol_equal(const union lval* a, const union lval* b);

#endif
