#ifndef SQUEAKY_LENV_H_INCLUDED
#define SQUEAKY_LENV_H_INCLUDED

#include "lobj.h"

struct lenv {
    long count;
    char** symbols;
    struct lobj** objects;
};

struct lenv* lenv_make(void);
void lenv_free(struct lenv* env);

struct lobj* lenv_get(const struct lenv* env, const struct lobj* k);
void lenv_put(struct lenv* env, const struct lobj* k, const struct lobj* v);

#endif
