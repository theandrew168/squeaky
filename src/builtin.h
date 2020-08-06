#ifndef SQUEAKY_BUILTIN_H_INCLUDED
#define SQUEAKY_BUILTIN_H_INCLUDED

#include "value.h"

struct value* builtin_is_boolean(struct value* args);
struct value* builtin_is_number(struct value* args);
struct value* builtin_is_string(struct value* args);
struct value* builtin_is_symbol(struct value* args);
struct value* builtin_is_pair(struct value* args);
struct value* builtin_is_procedure(struct value* args);
struct value* builtin_is_window(struct value* args);
struct value* builtin_is_event(struct value* args);
struct value* builtin_is_error(struct value* args);

struct value* builtin_add(struct value* args);
struct value* builtin_sub(struct value* args);
struct value* builtin_mul(struct value* args);
struct value* builtin_div(struct value* args);
struct value* builtin_equal(struct value* args);
struct value* builtin_less(struct value* args);
struct value* builtin_less_equal(struct value* args);
struct value* builtin_greater(struct value* args);
struct value* builtin_greater_equal(struct value* args);

struct value* builtin_is_eq(struct value* args);
struct value* builtin_not(struct value* args);

struct value* builtin_display(struct value* args);
struct value* builtin_newline(struct value* args);

struct value* builtin_delay(struct value* args);
struct value* builtin_make_window(struct value* args);
struct value* builtin_window_clear(struct value* args);
struct value* builtin_window_draw_line(struct value* args);
struct value* builtin_window_present(struct value* args);
struct value* builtin_event_poll(struct value* args);
struct value* builtin_event_type(struct value* args);

#endif
