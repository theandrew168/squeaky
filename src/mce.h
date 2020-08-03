#ifndef SQUEAKY_MCE_H_INCLUDED
#define SQUEAKY_MCE_H_INCLUDED

#include "value.h"

struct value* mce_eval(struct value* exp, struct value* env);
struct value* mce_apply(struct value* proc, struct value* args);

struct value* mce_load(struct value* args, struct value* env);

#endif
