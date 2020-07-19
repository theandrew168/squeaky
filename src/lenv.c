#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lenv.h"
#include "lobj.h"

struct lenv*
lenv_make(void)
{
    struct lenv* env = malloc(sizeof(struct lenv));
    env->count = 0;
    env->symbols = NULL;
    env->objects = NULL;
    return env;
}

void
lenv_free(struct lenv* env)
{
    assert(env != NULL);

    for (long i = 0; i < env->count; i++) {
        free(env->symbols[i]);
        lobj_free(env->objects[i]);
    }
    free(env->symbols);
    free(env->objects);
    free(env);
}

struct lobj*
lenv_get(const struct lenv* env, const struct lobj* k)
{
    for (long i = 0; i < env->count; i++) {
        if (strcmp(env->symbols[i], k->symbol) == 0) {
            return lobj_copy(env->objects[i]);
        }
    }

    return lobj_make_error("unbound symbol '%s'", k->symbol);
}

void
lenv_put(struct lenv* env, const struct lobj* k, const struct lobj* v)
{
    // check for existing key
    for (long i = 0; i < env->count; i++) {
        // replace object if it already exists
        if (strcmp(env->symbols[i], k->symbol) == 0) {
            lobj_free(env->objects[i]);
            env->objects[i] = lobj_copy(v);
            return;
        }
    }

    // alloc slot for new entry
    env->count++;
    env->objects = realloc(env->objects, env->count * sizeof(struct lobj*));
    env->symbols = realloc(env->symbols, env->count * sizeof(char*));

    // copy in the new object and its symbol
    env->objects[env->count - 1] = lobj_copy(v);
    env->symbols[env->count - 1] = malloc(strlen(k->symbol) + 1);
    strcpy(env->symbols[env->count - 1], k->symbol);
}
