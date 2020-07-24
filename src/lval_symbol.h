#ifndef SQUEAKY_LVAL_SYMBOL_H_INCLUDED
#define SQUEAKY_LVAL_SYMBOL_H_INCLUDED

#include <stdbool.h>

struct lval;

struct lval_symbol {
    char* symbol;
};

#define AS_SYMBOL(val) ((val)->as.symbol)

bool lval_symbol_init(struct lval* val, const char* symbol);
void lval_symbol_free(struct lval* val);
void lval_symbol_copy(const struct lval* val, struct lval* copy);

void lval_symbol_print(const struct lval* val);
bool lval_symbol_equal(const struct lval* a, const struct lval* b);

#endif
