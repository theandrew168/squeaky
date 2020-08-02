#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "env.h"
#include "value.h"

static struct value*
make_test_env(void)
{
    struct value* vars = list_make(
        value_make_symbol("foo", 3),
        value_make_symbol("bar", 3),
        NULL);
    struct value* vals = list_make(
        value_make_number(123),
        value_make_number(42),
        NULL);
    struct value* env = env_bind(vars, vals, NULL);

    return env;
}

bool
test_env_bind(void)
{
    struct value* base_env = make_test_env();

    // bind a new value for "foo"
    struct value* vars = list_make(
        value_make_symbol("foo", 3),
        NULL);
    struct value* vals = list_make(
        value_make_number(321),
        NULL);
    struct value* env = env_bind(vars, vals, base_env);

    // make sure to get the new foo
    struct value* foo = env_lookup(value_make_symbol("foo", 3), env);
    if (foo->as.number != 321) {
        fprintf(stderr, "env_lookup after env_bind found wrong 'foo'\n");
        return false;
    }

    // make sure to get the old bar
    struct value* bar = env_lookup(value_make_symbol("bar", 3), env);
    if (bar->as.number != 42) {
        fprintf(stderr, "env_lookup after env_bind found wrong 'bar'\n");
        return false;
    }

    // bind a new value for "foo" and "baz"
    struct value* new_vars = list_make(
        value_make_symbol("foo", 3),
        value_make_symbol("bar", 3),
        NULL);
    struct value* new_vals = list_make(
        value_make_number(111),
        value_make_number(222),
        NULL);
    struct value* new_env = env_bind(new_vars, new_vals, env);

    // make sure to get the new foo
    foo = env_lookup(value_make_symbol("foo", 3), new_env);
    if (foo->as.number != 111) {
        fprintf(stderr, "env_lookup after 2x env_bind found wrong 'foo'\n");
        return false;
    }

    // make sure to get the new bar
    bar = env_lookup(value_make_symbol("bar", 3), new_env);
    if (bar->as.number != 222) {
        fprintf(stderr, "env_lookup after 2x env_bind found wrong 'bar'\n");
        return false;
    }

    return true;
}

bool
test_env_lookup(void)
{
    struct value* env = make_test_env();

    // make sure to find foo
    struct value* foo = env_lookup(value_make_symbol("foo", 3), env);
    if (foo->as.number != 123) {
        fprintf(stderr, "env_lookup failed to find 'foo'\n");
        return false;
    }

    // make sure lookup of missing key returns an error
    struct value* missing = env_lookup(value_make_symbol("missing", 7), env);
    if (!value_is_error(missing)) {
        fprintf(stderr, "env_lookup failed to return error if not found\n");
        return false;
    }

    return true;
}

bool
test_env_update(void)
{
    struct value* env = make_test_env();

    // update foo
    struct value* res = env_update(value_make_symbol("foo", 3), value_make_number(111), env);
    if (value_is_error(res)) {
        fprintf(stderr, "env_update failed a valid update\n");
        return false;
    }

    // make sure to get the new value for foo
    struct value* foo = env_lookup(value_make_symbol("foo", 3), env);
    if (foo->as.number != 111) {
        fprintf(stderr, "env_lookup after env_update found wrong 'foo'\n");
        return false;
    }

    // ensure update of missing key returns an error
    res = env_update(value_make_symbol("missing", 7), value_make_number(42), env);
    if (!value_is_error(res)) {
        fprintf(stderr, "env_update failed to return error if not found\n");
        return false;
    }

    return true;
}

bool
test_env_define(void)
{
    struct value* env = make_test_env();

    // redefine foo in the current frame
    struct value* res = env_define(value_make_symbol("foo", 3), value_make_number(111), env);
    if (value_is_error(res)) {
        fprintf(stderr, "env_define failed a valid redefine\n");
        return false;
    }

    // ensure to get the new value for foo
    struct value* foo = env_lookup(value_make_symbol("foo", 3), env);
    if (foo->as.number != 111) {
        fprintf(stderr, "env_lookup after env_define found wrong 'foo'\n");
        return false;
    }

    // define a new var 'baz' in the current frame
    res = env_define(value_make_symbol("baz", 3), value_make_number(333), env);
    if (value_is_error(res)) {
        fprintf(stderr, "env_define failed to define 'baz'\n");
        return false;
    }

    // lookup 'baz'
    struct value* baz = env_lookup(value_make_symbol("baz", 3), env);
    if (baz->as.number != 333) {
        fprintf(stderr, "env_lookup after env_define failed to find 'baz'\n");
        return false;
    }

    // create a new frame with a new value for 'foo'
    struct value* vars = list_make(
        value_make_symbol("foo", 3),
        NULL);
    struct value* vals = list_make(
        value_make_number(321),
        NULL);
    struct value* new_env = env_bind(vars, vals, env);

    // ensure to get the new frame's value for foo
    foo = env_lookup(value_make_symbol("foo", 3), new_env);
    if (foo->as.number != 321) {
        fprintf(stderr, "env_lookup in new frame found wrong 'foo'\n");
        return false;
    }

    // ensure to get the old frame's value for foo is unchanged
    foo = env_lookup(value_make_symbol("foo", 3), env);
    if (foo->as.number != 111) {
        fprintf(stderr, "env_lookup in old frame found wrong 'foo'\n");
        return false;
    }

    return true;
}
