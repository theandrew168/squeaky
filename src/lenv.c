#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lenv.h"
#include "lval.h"

struct lenv*
lenv_make(void)
{
    struct lenv* env = malloc(sizeof(struct lenv));
    env->parent = NULL;
    env->count = 0;
    env->symbols = NULL;
    env->values = NULL;
    return env;
}

void
lenv_free(struct lenv* env)
{
    assert(env != NULL);

    for (long i = 0; i < env->count; i++) {
        free(env->symbols[i]);
        lval_free(env->values[i]);
    }
    free(env->symbols);
    free(env->values);
    free(env);
}

struct lval*
lenv_get(const struct lenv* env, const struct lval* k)
{
    for (long i = 0; i < env->count; i++) {
        if (strcmp(env->symbols[i], k->symbol) == 0) {
            return lval_copy(env->values[i]);
        }
    }

    if (env->parent != NULL) {
        return lenv_get(env->parent, k);
    } else {
        return lval_make_error("unbound symbol '%s'", k->symbol);
    }
}

void
lenv_put(struct lenv* env, const struct lval* k, const struct lval* v)
{
    // check for existing key
    for (long i = 0; i < env->count; i++) {
        // replace value if it already exists
        if (strcmp(env->symbols[i], k->symbol) == 0) {
            lval_free(env->values[i]);
            env->values[i] = lval_copy(v);
            return;
        }
    }

    // alloc slot for new entry
    env->count++;
    env->values = realloc(env->values, env->count * sizeof(struct lval*));
    env->symbols = realloc(env->symbols, env->count * sizeof(char*));

    // copy in the new value and its symbol
    env->values[env->count - 1] = lval_copy(v);
    env->symbols[env->count - 1] = malloc(strlen(k->symbol) + 1);
    strcpy(env->symbols[env->count - 1], k->symbol);
}

void
lenv_def(struct lenv* env, const struct lval* k, const struct lval* v)
{
    while (env->parent != NULL) env = env->parent;
    lenv_put(env, k, v);
}

struct lenv*
lenv_copy(const struct lenv* env)
{
    struct lenv* new = malloc(sizeof(struct lenv));
    new->parent = env->parent;
    new->count = env->count;
    new->symbols = malloc(env->count * sizeof(char*));
    new->values = malloc(env->count * sizeof(struct lval*));
    for (long i = 0; i < env->count; i++) {
        new->symbols[i] = malloc(strlen(env->symbols[i]) + 1);
        strcpy(new->symbols[i], env->symbols[i]);
        new->values[i] = lval_copy(env->values[i]);
    }
    return new;
}
