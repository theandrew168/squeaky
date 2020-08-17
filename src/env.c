#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "env.h"
#include "list.h"
#include "value.h"

#define make_frame(vars, vals) (cons(vars, vals))
#define frame_vars(frame) (car(frame))
#define frame_vals(frame) (cdr(frame))

static struct value*
frame_lookup(struct value* var, struct value* vars, struct value* vals)
{
    if (value_is_empty_list(vars) && value_is_empty_list(vals)) return NULL;
    assert(!value_is_empty_list(vars) && "env frame has mismatched vars and vals");
    assert(!value_is_empty_list(vals) && "env frame has mismatched vars and vals");

    if (value_is_equal(var, car(vars))) return car(vals);
    return frame_lookup(var, cdr(vars), cdr(vals));
}

static struct value*
frame_update(struct value* var, struct value* val, struct value* vars, struct value* vals)
{
    if (value_is_empty_list(vars) && value_is_empty_list(vals)) return NULL;
    assert(!value_is_empty_list(vars) && "env frame has mismatched vars and vals");
    assert(!value_is_empty_list(vals) && "env frame has mismatched vars and vals");

    if (value_is_equal(var, car(vars))) {
        vals->as.pair.car = val;
        return value_make_empty_list();
    }

    return frame_update(var, val, cdr(vars), cdr(vals));
}

static struct value*
frame_add_binding(struct value* var, struct value* val, struct value* frame)
{
    assert(frame != NULL);

    frame->as.pair.car = cons(var, car(frame));
    frame->as.pair.cdr = cons(val, cdr(frame));
    return value_make_empty_list();
}

#define first_frame(env) (car(env))
#define rest_frames(env) (cdr(env))

struct value*
env_empty(void)
{
    return env_extend(
        value_make_empty_list(),
        value_make_empty_list(),
        value_make_empty_list());
}

struct value*
env_extend(struct value* vars, struct value* vals, struct value* env)
{
    long vars_len = list_length(vars);
    long vals_len = list_length(vals);
    assert(vars_len == vals_len && "mismatched number of vars/vals supplied to env_extend");

    return cons(make_frame(vars, vals), env);
}

struct value*
env_lookup(struct value* var, struct value* env)
{
    assert(value_is_symbol(var) && "non-symbol key passed to env_lookup");

    if (value_is_empty_list(env)) {
        fprintf(stderr, "unbound variable: %s\n", var->as.symbol);
        exit(EXIT_FAILURE);
    }

    struct value* frame = first_frame(env);
    struct value* val = frame_lookup(var, frame_vars(frame), frame_vals(frame));
    if (val != NULL) return val;
    return env_lookup(var, rest_frames(env));
}

struct value*
env_update(struct value* var, struct value* val, struct value* env)
{
    assert(value_is_symbol(var) && "non-symbol key passed to env_update");

    if (value_is_empty_list(env)) {
        fprintf(stderr, "unbound variable: %s\n", var->as.symbol);
        exit(EXIT_FAILURE);
    }

    struct value* frame = first_frame(env);
    struct value* existing_val = frame_lookup(var, frame_vars(frame), frame_vals(frame));
    if (existing_val != NULL) return frame_update(var, val, frame_vars(frame), frame_vals(frame));
    return env_update(var, val, rest_frames(env));
}

struct value*
env_define(struct value* var, struct value* val, struct value* env)
{
    assert(value_is_symbol(var) && "non-symbol key passed to env_define");

    struct value* frame = first_frame(env);
    struct value* existing_val = frame_lookup(var, frame_vars(frame), frame_vals(frame));
    if (existing_val != NULL) return frame_update(var, val, frame_vars(frame), frame_vals(frame));
    return frame_add_binding(var, val, frame);
}
