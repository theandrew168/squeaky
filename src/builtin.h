#ifndef SQUEAKY_BUILTIN_H_INCLUDED
#define SQUEAKY_BUILTIN_H_INCLUDED

#include "value.h"

struct value* builtin_is_boolean(struct value* args);
struct value* builtin_is_symbol(struct value* args);
struct value* builtin_is_procedure(struct value* args);
struct value* builtin_is_pair(struct value* args);
struct value* builtin_is_number(struct value* args);
struct value* builtin_is_string(struct value* args);
struct value* builtin_is_window(struct value* args);

struct value* builtin_make_window(struct value* args);

struct value* builtin_plus(struct value* args);
struct value* builtin_multiply(struct value* args);

#endif
