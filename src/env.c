#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "env.h"
#include "value.h"

// TODO: this impl violates "The Law of Cons"
// rewrite as two lists per frame: one for vars and one for vals

// create a new, empty frame on top of the given env
struct value*
env_frame(struct value* env)
{
    return cons(NULL, env);
}

static struct value*
pair_up(struct value* vars, struct value* vals)
{
    if (vars == NULL && vals == NULL) return NULL;
    if (vars == NULL) {
        fprintf(stderr, "too many arguments given to lambda... crash!\n");
        return value_make_error("too many arguments given to lambda");
    }
    if (vals == NULL) {
        fprintf(stderr, "too few arguments given to lambda... crash!\n");
        return value_make_error("too few arguments given to lambda");
    }

    return cons(cons(car(vars), car(vals)),
                pair_up(cdr(vars), cdr(vals)));
}

// create a new env with a leading frame that binds vars to vals
struct value*
env_bind(struct value* vars, struct value* vals, struct value* env)
{
    return cons(pair_up(vars, vals), env);
}

// search a frame for a given symbol, return (sym, value) pair if found
static struct value*
assq(struct value* sym, struct value* frame)
{
    if (frame == NULL) return NULL;

    // TODO: type assertion for the symbols?

    if (strcmp(sym->as.symbol, caar(frame)->as.symbol) == 0) {
        return car(frame);
    } else {
        return assq(sym, cdr(frame));
    }
}

// recur search all frames
struct value*
env_lookup(struct value* sym, struct value* env)
{
    if (env == NULL) {
        return value_make_error("unbound variable");
    }

    struct value* vcell = assq(sym, car(env));
    if (vcell != NULL) {
        return cdr(vcell);
    } else {
        return env_lookup(sym, cdr(env));
    }
}

// recur serach and update, error if not found
struct value*
env_update(struct value* sym, struct value* value, struct value* env)
{
    if (env == NULL) {
        return value_make_error("unbound variable");
    }

    struct value* vcell = assq(sym, car(env));
    if (vcell != NULL) {
        vcell->as.pair.cdr = value;
        return value_make_pair(NULL, NULL);
    } else {
        return env_lookup(sym, cdr(env));
    }
}

// search first frame only, update if found, add if not
struct value*
env_define(struct value* sym, struct value* value, struct value* env)
{
    struct value* vcell = assq(sym, car(env));
    if (vcell != NULL) {
        // update exiting vcell
        vcell->as.pair.cdr = value;
    } else {
        // prepend a new vcell to this frame
        env->as.pair.car = cons(cons(sym, value), car(env));
    }

    return value_make_pair(NULL, NULL);
}

void
env_print(const struct value* env)
{
    if (env == NULL) {
        printf("env| == done ==\n");
        return;
    }

    printf("env| == frame == \n");
    struct value* frame = car(env);
    while (frame != NULL) {
        struct value* vcell = car(frame);
        printf("env| %s: ", car(vcell)->as.symbol);
        value_write(cdr(vcell));
        printf("\n");

        frame = cdr(frame);
    }

    env_print(cdr(env));
}
