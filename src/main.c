#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "env.h"
#include "mce.h"
#include "value.h"

// 1. Define core data structure (linkable tagged union)
// 2. Convert text to this data structure (read)
// 3. Evaluate the data structure (eval/apply)

// TODO: Impl alt define form for easier lambdas
// TODO: Add read_list helper to read func
// TODO: Add special form "if"
// TODO: Add assert helpers for builtins (arity and types)
// TODO: Add a simple ref counted GC / memory management

int
main(int argc, char* argv[])
{
    struct value* vars = list_make(
        value_make_symbol("boolean?", 8),
        value_make_symbol("symbol?", 7),
        value_make_symbol("procedure?", 10),
        value_make_symbol("pair?", 5),
        value_make_symbol("number?", 7),
        value_make_symbol("string?", 7),
        value_make_symbol("+", 1),
        value_make_symbol("*", 1),
        NULL);
    struct value* vals = list_make(
        value_make_builtin(builtin_is_boolean),
        value_make_builtin(builtin_is_symbol),
        value_make_builtin(builtin_is_procedure),
        value_make_builtin(builtin_is_pair),
        value_make_builtin(builtin_is_number),
        value_make_builtin(builtin_is_string),
        value_make_builtin(builtin_plus),
        value_make_builtin(builtin_multiply),
        NULL);
    struct value* env = env_bind(vars, vals, NULL);

    printf("> ");
    char line[512] = { 0 };
    while (fgets(line, sizeof(line), stdin) != NULL) {
        long consumed = 0;
        struct value* exp = value_read(line, &consumed);
//        value_write(exp);
//        printf("\n");

        struct value* res = mce_eval(exp, env);
        value_write(res);
        printf("\n");

        printf("> ");
    }
}
