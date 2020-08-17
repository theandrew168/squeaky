#ifndef SQUEAKY_BUILTIN_H_INCLUDED
#define SQUEAKY_BUILTIN_H_INCLUDED

#include "value.h"

// R5RS 6.1: Equivalence Predicates
struct value* builtin_is_eq(struct value* args);
struct value* builtin_is_eqv(struct value* args);
struct value* builtin_is_equal(struct value* args);

// R5RS 6.2.5: Numerical Operations
struct value* builtin_is_number(struct value* args);
struct value* builtin_equal(struct value* args);
struct value* builtin_less(struct value* args);
struct value* builtin_greater(struct value* args);
struct value* builtin_less_equal(struct value* args);
struct value* builtin_greater_equal(struct value* args);
struct value* builtin_add(struct value* args);
struct value* builtin_mul(struct value* args);
struct value* builtin_sub(struct value* args);
struct value* builtin_div(struct value* args);

// R5RS 6.3.1: Booleans
struct value* builtin_is_boolean(struct value* args);

// R5RS 6.3.2: Pairs and Lists
struct value* builtin_is_pair(struct value* args);
struct value* builtin_cons(struct value* args);
struct value* builtin_car(struct value* args);
struct value* builtin_cdr(struct value* args);
struct value* builtin_set_car(struct value* args);
struct value* builtin_set_cdr(struct value* args);
struct value* builtin_is_null(struct value* args);

// R5RS 6.3.3: Symbols
struct value* builtin_is_symbol(struct value* args);

// R5RS 6.3.5: Strings
struct value* builtin_is_string(struct value* args);

// R5RS 6.4: Control Features
struct value* builtin_is_procedure(struct value* args);

// R5RS 6.6.1: Ports
struct value* builtin_is_input_port(struct value* args);
struct value* builtin_is_output_port(struct value* args);
struct value* builtin_current_input_port(struct value* args);
struct value* builtin_current_output_port(struct value* args);
struct value* builtin_open_input_file(struct value* args);
struct value* builtin_open_output_file(struct value* args);
struct value* builtin_close_input_port(struct value* args);
struct value* builtin_close_output_port(struct value* args);

// R5RS 6.6.2: Input
struct value* builtin_read(struct value* args);
struct value* builtin_read_char(struct value* args);
struct value* builtin_peek_char(struct value* args);
struct value* builtin_is_eof_object(struct value* args);
struct value* builtin_is_char_ready(struct value* args);

// R5RS 6.6.3: Output
struct value* builtin_write(struct value* args);
struct value* builtin_display(struct value* args);
struct value* builtin_newline(struct value* args);
struct value* builtin_write_char(struct value* args);

/* Squeaky Extensions */

// General Utilities
struct value* builtin_sleep(struct value* args);

// Windows
struct value* builtin_is_window(struct value* args);
struct value* builtin_make_window(struct value* args);
struct value* builtin_window_clear(struct value* args);
struct value* builtin_window_draw_line(struct value* args);
struct value* builtin_window_present(struct value* args);

// Events
struct value* builtin_is_event(struct value* args);
struct value* builtin_window_event_poll(struct value* args);
struct value* builtin_window_event_type(struct value* args);

#endif
