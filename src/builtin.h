#ifndef SQUEAKY_BUILTIN_H_INCLUDED
#define SQUEAKY_BUILTIN_H_INCLUDED

#include "value.h"
#include "vm.h"

// R5RS 6.1: Equivalence Predicates
struct value* builtin_is_eq(struct vm* vm, struct value* args);
struct value* builtin_is_eqv(struct vm* vm, struct value* args);
struct value* builtin_is_equal(struct vm* vm, struct value* args);

// R5RS 6.2.5: Numerical Operations
struct value* builtin_is_number(struct vm* vm, struct value* args);
struct value* builtin_equal(struct vm* vm, struct value* args);
struct value* builtin_less(struct vm* vm, struct value* args);
struct value* builtin_greater(struct vm* vm, struct value* args);
struct value* builtin_less_equal(struct vm* vm, struct value* args);
struct value* builtin_greater_equal(struct vm* vm, struct value* args);
struct value* builtin_add(struct vm* vm, struct value* args);
struct value* builtin_mul(struct vm* vm, struct value* args);
struct value* builtin_sub(struct vm* vm, struct value* args);
struct value* builtin_div(struct vm* vm, struct value* args);

// R5RS 6.3.1: Booleans
struct value* builtin_is_boolean(struct vm* vm, struct value* args);

// R5RS 6.3.2: Pairs and Lists
struct value* builtin_is_pair(struct vm* vm, struct value* args);
struct value* builtin_cons(struct vm* vm, struct value* args);
struct value* builtin_car(struct vm* vm, struct value* args);
struct value* builtin_cdr(struct vm* vm, struct value* args);
struct value* builtin_set_car(struct vm* vm, struct value* args);
struct value* builtin_set_cdr(struct vm* vm, struct value* args);
struct value* builtin_is_null(struct vm* vm, struct value* args);

// R5RS 6.3.3: Symbols
struct value* builtin_is_symbol(struct vm* vm, struct value* args);

// R5RS 6.3.5: Strings
struct value* builtin_is_string(struct vm* vm, struct value* args);

// R5RS 6.4: Control Features
struct value* builtin_is_procedure(struct vm* vm, struct value* args);

// R5RS 6.6.1: Ports
struct value* builtin_is_input_port(struct vm* vm, struct value* args);
struct value* builtin_is_output_port(struct vm* vm, struct value* args);
struct value* builtin_current_input_port(struct vm* vm, struct value* args);
struct value* builtin_current_output_port(struct vm* vm, struct value* args);
struct value* builtin_open_input_file(struct vm* vm, struct value* args);
struct value* builtin_open_output_file(struct vm* vm, struct value* args);
struct value* builtin_close_input_port(struct vm* vm, struct value* args);
struct value* builtin_close_output_port(struct vm* vm, struct value* args);

// R5RS 6.6.2: Input
struct value* builtin_read(struct vm* vm, struct value* args);
struct value* builtin_read_char(struct vm* vm, struct value* args);
struct value* builtin_peek_char(struct vm* vm, struct value* args);
struct value* builtin_is_eof_object(struct vm* vm, struct value* args);
struct value* builtin_is_char_ready(struct vm* vm, struct value* args);

// R5RS 6.6.3: Output
struct value* builtin_write(struct vm* vm, struct value* args);
struct value* builtin_display(struct vm* vm, struct value* args);
struct value* builtin_newline(struct vm* vm, struct value* args);
struct value* builtin_write_char(struct vm* vm, struct value* args);

/* Squeaky Extensions */

// General Utilities
struct value* builtin_sleep(struct vm* vm, struct value* args);

// Windows
struct value* builtin_is_window(struct vm* vm, struct value* args);
struct value* builtin_make_window(struct vm* vm, struct value* args);
struct value* builtin_window_clear(struct vm* vm, struct value* args);
struct value* builtin_window_draw_line(struct vm* vm, struct value* args);
struct value* builtin_window_present(struct vm* vm, struct value* args);

// Events
struct value* builtin_is_event(struct vm* vm, struct value* args);
struct value* builtin_window_event_poll(struct vm* vm, struct value* args);
struct value* builtin_window_event_type(struct vm* vm, struct value* args);
struct value* builtin_window_event_key(struct vm* vm, struct value* args);

#endif
