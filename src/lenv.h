#ifndef SQUEAKY_LENV_H_INCLUDED
#define SQUEAKY_LENV_H_INCLUDED

#include "lval.h"

struct lenv {
    struct lenv* parent;
    long count;
    char** symbols;
    struct lval** values;
};

struct lenv* lenv_make(void);
void lenv_free(struct lenv* env);

struct lval* lenv_get(const struct lenv* env, const struct lval* k);
void lenv_put(struct lenv* env, const struct lval* k, const struct lval* v);
void lenv_def(struct lenv* env, const struct lval* k, const struct lval* v);

struct lenv* lenv_copy(const struct lenv* env);

#endif
