#ifndef SQUEAKY_MCE_H_INCLUDED
#define SQUEAKY_MCE_H_INCLUDED

#include "value.h"
#include "vm.h"

struct value* mce_eval(struct vm* vm, struct value* exp, struct value* env);
struct value* mce_apply(struct vm* vm, struct value* proc, struct value* args);

// these funcs won't actually be called, they are just markers for the MCE
struct value* mce_builtin_eval(struct vm* vm, struct value* args);
struct value* mce_builtin_apply(struct vm* vm, struct value* args);

#endif
