#ifndef SQUEAKY_ENV_H_INCLUDED
#define SQUEAKY_ENV_H_INCLUDED

#include "value.h"

struct env {
    struct env* parent;
    long count;
    char** symbols;
    struct value** values;
};

struct env* env_make(void);
void env_free(struct env* env);

struct value* env_get(const struct env* env, const char* k);
void env_put(struct env* env, const char* k, struct value* v);
void env_def(struct env* env, const char* k, struct value* v);

void env_print(const struct env* env);

#endif
