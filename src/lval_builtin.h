#ifndef SQUEAKY_LVAL_BUILTIN_H_INCLUDED
#define SQUEAKY_LVAL_BUILTIN_H_INCLUDED

#include <stdbool.h>

#include "lbuiltin.h"

struct lval;

bool lval_builtin_init(struct lval* val, lbuiltin builtin);
void lval_builtin_free(struct lval* val);
void lval_builtin_copy(const struct lval* val, struct lval* copy);

void lval_builtin_print(const struct lval* val);
bool lval_builtin_equal(const struct lval* a, const struct lval* b);

#endif
