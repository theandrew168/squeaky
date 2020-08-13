#ifndef SQUEAKY_MCE_H_INCLUDED
#define SQUEAKY_MCE_H_INCLUDED

#include "value.h"

struct value* mce_eval(struct value* exp, struct value* env);
struct value* mce_apply(struct value* proc, struct value* args);

// these funcs won't actually be called, they are just markers for the MCE
struct value* mce_builtin_eval(struct value* args);
struct value* mce_builtin_apply(struct value* args);

#endif
