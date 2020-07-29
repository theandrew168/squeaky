#ifndef SQUEAKY_ENV_H_INCLUDED
#define SQUEAKY_ENV_H_INCLUDED

#include "value.h"

struct value* env_get(const struct value* env, const struct value* k);
void env_set(struct value* env, struct value* k, struct value* v);

void env_print(const struct value* env);

#endif
