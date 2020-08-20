#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "env.h"
#include "list.h"
#include "value.h"

#define make_frame(vm, vars, vals) (vm_make_pair(vm, vars, vals))
#define frame_vars(frame) (CAR(frame))
#define frame_vals(frame) (CDR(frame))

static struct value*
frame_lookup(struct vm* vm, struct value* var, struct value* vars, struct value* vals)
{
    if (value_is_empty_list(vars) && value_is_empty_list(vals)) return NULL;
    assert(!value_is_empty_list(vars) && "env frame has mismatched vars and vals");
    assert(!value_is_empty_list(vals) && "env frame has mismatched vars and vals");

    if (value_is_equal(var, CAR(vars))) return CAR(vals);
    return frame_lookup(vm, var, CDR(vars), CDR(vals));
}

static struct value*
frame_update(struct vm* vm, struct value* var, struct value* val, struct value* vars, struct value* vals)
{
    if (value_is_empty_list(vars) && value_is_empty_list(vals)) return NULL;
    assert(!value_is_empty_list(vars) && "env frame has mismatched vars and vals");
    assert(!value_is_empty_list(vals) && "env frame has mismatched vars and vals");

    if (value_is_equal(var, CAR(vars))) {
        vals->as.pair.car = val;
        return vm_make_empty_list(vm);
    }

    return frame_update(vm, var, val, CDR(vars), CDR(vals));
}

static struct value*
frame_add_binding(struct vm* vm, struct value* var, struct value* val, struct value* frame)
{
    assert(frame != NULL);

    frame->as.pair.car = vm_make_pair(vm, var, CAR(frame));
    frame->as.pair.cdr = vm_make_pair(vm, val, CDR(frame));
    return vm_make_empty_list(vm);
}

#define first_frame(env) (CAR(env))
#define rest_frames(env) (CDR(env))

struct value*
env_empty(struct vm* vm)
{
    return env_extend(vm,
        vm_make_empty_list(vm),
        vm_make_empty_list(vm),
        vm_make_empty_list(vm));
}

struct value*
env_extend(struct vm* vm, struct value* vars, struct value* vals, struct value* env)
{
    long vars_len = list_length(vars);
    long vals_len = list_length(vals);
    assert(vars_len == vals_len && "mismatched number of vars/vals supplied to env_extend");

    return vm_make_pair(vm, make_frame(vm, vars, vals), env);
}

struct value*
env_lookup(struct vm* vm, struct value* var, struct value* env)
{
    assert(value_is_symbol(var) && "non-symbol key passed to env_lookup");

    if (value_is_empty_list(env)) {
        fprintf(stderr, "unbound variable: %s\n", var->as.symbol);
        exit(EXIT_FAILURE);
    }

    struct value* frame = first_frame(env);
    struct value* val = frame_lookup(vm, var, frame_vars(frame), frame_vals(frame));
    if (val != NULL) return val;
    return env_lookup(vm, var, rest_frames(env));
}

struct value*
env_update(struct vm* vm, struct value* var, struct value* val, struct value* env)
{
    assert(value_is_symbol(var) && "non-symbol key passed to env_update");

    if (value_is_empty_list(env)) {
        fprintf(stderr, "unbound variable: %s\n", var->as.symbol);
        exit(EXIT_FAILURE);
    }

    struct value* frame = first_frame(env);
    struct value* existing_val = frame_lookup(vm, var, frame_vars(frame), frame_vals(frame));
    if (existing_val != NULL) return frame_update(vm, var, val, frame_vars(frame), frame_vals(frame));
    return env_update(vm, var, val, rest_frames(env));
}

struct value*
env_define(struct vm* vm, struct value* var, struct value* val, struct value* env)
{
    assert(value_is_symbol(var) && "non-symbol key passed to env_define");

    struct value* frame = first_frame(env);
    struct value* existing_val = frame_lookup(vm, var, frame_vars(frame), frame_vals(frame));
    if (existing_val != NULL) return frame_update(vm, var, val, frame_vars(frame), frame_vals(frame));
    return frame_add_binding(vm, var, val, frame);
}
