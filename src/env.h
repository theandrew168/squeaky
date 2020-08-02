#ifndef SQUEAKY_ENV_H_INCLUDED
#define SQUEAKY_ENV_H_INCLUDED

#include "value.h"

struct value* env_bind(struct value* vars, struct value* vals, struct value* env);
struct value* env_lookup(struct value* sym, struct value* env);
struct value* env_update(struct value* sym, struct value* value, struct value* env);
struct value* env_define(struct value* sym, struct value* value, struct value* env);
void env_print(const struct value* env);

#endif
