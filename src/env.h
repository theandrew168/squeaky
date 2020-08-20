#ifndef SQUEAKY_ENV_H_INCLUDED
#define SQUEAKY_ENV_H_INCLUDED

#include "value.h"
#include "vm.h"

struct value* env_empty(struct vm* vm);
struct value* env_extend(struct vm* vm, struct value* vars, struct value* vals, struct value* env);
struct value* env_lookup(struct vm* vm, struct value* var, struct value* env);
struct value* env_update(struct vm* vm, struct value* var, struct value* val, struct value* env);
struct value* env_define(struct vm* vm, struct value* var, struct value* val, struct value* env);

#endif
