#ifndef SQUEAKY_LVAL_BUILTIN_H_INCLUDED
#define SQUEAKY_LVAL_BUILTIN_H_INCLUDED

#include <stdbool.h>

#include "lbuiltin.h"

union lval;

struct lval_builtin {
    int type;
    lbuiltin builtin;
};

#define AS_BUILTIN(val) ((struct lval_builtin*)(val))
#define AS_CONST_BUILTIN(val) ((const struct lval_builtin*)(val))

bool lval_builtin_init(union lval* val, lbuiltin builtin);
void lval_builtin_free(union lval* val);
void lval_builtin_copy(const union lval* val, union lval* copy);

void lval_builtin_print(const union lval* val);
bool lval_builtin_equal(const union lval* a, const union lval* b);

#endif
