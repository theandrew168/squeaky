#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "env.h"
#include "value.h"

struct value*
env_get(const struct value* env, const struct value* k)
{
    assert(env != NULL);
    assert(k != NULL);
}

void
env_set(struct value* env, struct value* k, struct value* v)
{
    assert(env != NULL);
    assert(k != NULL);
    assert(v != NULL);
}

void
env_print(const struct value* env)
{
    assert(env != NULL);
}
