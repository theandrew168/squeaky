#ifndef SQUEAKY_LVAL_BUILTIN_H_INCLUDED
#define SQUEAKY_LVAL_BUILTIN_H_INCLUDED

#include <stdbool.h>

// TODO: where does the lbuiltin typedef go?
#include "lval.h"

struct lval_builtin {
    int type;
    lbuiltin builtin;
};

bool lval_builtin_init(struct lval_builtin* val, lbuiltin builtin);
void lval_builtin_free(struct lval_builtin* val);
void lval_builtin_copy(const struct lval_builtin* val, struct lval_builtin* copy);

void lval_builtin_print(const struct lval_builtin* val);
bool lval_builtin_equal(const struct lval_builtin* a, const struct lval_builtin* b);

#endif
