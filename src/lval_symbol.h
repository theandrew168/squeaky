#ifndef SQUEAKY_LVAL_SYMBOL_H_INCLUDED
#define SQUEAKY_LVAL_SYMBOL_H_INCLUDED

#include <stdbool.h>

struct lval_symbol {
    int type;
    char* symbol;
};

bool lval_symbol_init(struct lval_symbol* val, const char* symbol);
void lval_symbol_free(struct lval_symbol* val);
void lval_symbol_copy(const struct lval_symbol* val, struct lval_symbol* copy);

void lval_symbol_print(const struct lval_symbol* val);
bool lval_symbol_equal(const struct lval_symbol* a, const struct lval_symbol* b);

#endif
