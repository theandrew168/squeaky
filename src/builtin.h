#ifndef SQUEAKY_BUILTIN_H_INCLUDED
#define SQUEAKY_BUILTIN_H_INCLUDED

#include "value.h"

struct value* builtin_plus(struct value* args);
struct value* builtin_multiply(struct value* args);

#endif
