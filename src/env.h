#ifndef SQUEAKY_ENV_H_INCLUDED
#define SQUEAKY_ENV_H_INCLUDED

#include "value.h"

struct value* env_extend(struct value* vars, struct value* vals, struct value* env);
struct value* env_lookup(struct value* var, struct value* env);
struct value* env_update(struct value* var, struct value* val, struct value* env);
struct value* env_define(struct value* var, struct value* val, struct value* env);

#endif
