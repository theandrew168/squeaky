#ifndef SQUEAKY_LENV_H_INCLUDED
#define SQUEAKY_LENV_H_INCLUDED

#include "lval.h"

struct lenv {
    struct lenv* parent;
    long count;
    char** symbols;
    union lval** values;
};

struct lenv* lenv_make(void);
void lenv_free(struct lenv* env);

union lval* lenv_get(const struct lenv* env, const union lval* k);
void lenv_put(struct lenv* env, const union lval* k, const union lval* v);
void lenv_def(struct lenv* env, const union lval* k, const union lval* v);

struct lenv* lenv_copy(const struct lenv* env);

#endif
