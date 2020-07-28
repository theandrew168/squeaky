#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "value.h"

struct env*
env_make(void)
{
    struct env* env = malloc(sizeof(struct env));
    env->parent = NULL;
    env->count = 0;
    env->symbols = NULL;
    env->values = NULL;
    return env;
}

void
env_free(struct env* env)
{
    assert(env != NULL);

    for (long i = 0; i < env->count; i++) {
        free(env->symbols[i]);
        value_free(env->values[i]);
    }

    free(env->symbols);
    free(env->values);
    free(env);
}

struct value*
env_get(const struct env* env, const char* k)
{
    assert(env != NULL);
    assert(k != NULL);

    for (long i = 0; i < env->count; i++) {
        if (strcmp(env->symbols[i], k) == 0) {
            return env->values[i];
        }
    }

    if (env->parent != NULL) {
        return env_get(env->parent, k);
    }

    return NULL;
}

void
env_put(struct env* env, const char* k, struct value* v)
{
    assert(env != NULL);
    assert(k != NULL);
    assert(v != NULL);

    // check for existing key and replace if present
    for (long i = 0; i < env->count; i++) {
        if (strcmp(env->symbols[i], k) == 0) {
            env->values[i] = v;
            return;
        }
    }

    // else alloc room for a new key
    env->count++;
    env->symbols = realloc(env->symbols, env->count * sizeof(char*));
    env->values = realloc(env->values, env->count * sizeof(struct value*));

    // append the new symbol and value
    env->symbols[env->count - 1] = malloc(strlen(k) + 1);
    strcpy(env->symbols[env->count - 1], k);
    env->values[env->count - 1] = v;
}

void
env_def(struct env* env, const char* k, struct value* v)
{
    assert(env != NULL);
    assert(k != NULL);
    assert(v != NULL);

    // advance to the parent-most env before adding the new kv pair
    while (env->parent != NULL) env = env->parent;
    env_put(env, k, v);
}
