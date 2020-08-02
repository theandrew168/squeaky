#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "env.h"
#include "value.h"

// 1. Define core data structure (linkable tagged union)
// 2. Convert text to this data structure (read)
// 3. Evaluate the data structure (eval/apply)

// BUG: Calling nested lambda reads invalid memory
//  (define square (lambda (x) (* x x)))
//  (define quad (lambda (x) (* square(x) square(x))))
//  (quad 5)  ; gives unpredictable results

// TODO: Add env creation helper (list of [sym, func] structs?)
// TODO: Add read_list helper to read func
// TODO: Add special form "if"
// TODO: Add assert helpers for builtins (arity and types)
// TODO: Add a simple ref counted GC / memory management

struct value* eval(struct value* exp, struct value* env);
struct value* apply(struct value* proc, struct value* args);
struct value* evlist(struct value* exps, struct value* env);
struct value* evcond(struct value* exps, struct value* env);

struct value*
eval(struct value* exp, struct value* env)
{
    if (exp->type == VALUE_BOOLEAN) {
        return exp;
    } else if (exp->type == VALUE_NUMBER) {
        return exp;
    } else if (exp->type == VALUE_STRING) {
        return exp;
    } else if (exp->type == VALUE_ERROR) {
        return exp;
    } else if (exp->type == VALUE_SYMBOL) {
        return env_lookup(exp, env);
    } else if (strcmp(car(exp)->as.symbol, "quote") == 0) {
        return cadr(exp);
    } else if (strcmp(car(exp)->as.symbol, "lambda") == 0) {
        // this differs from SICP:
        // the lecture returns: ('closure (params body) env)
        // (lambda (x) (* x x))
//        struct value* lamb = value_make_lambda(cadr(exp), caddr(exp), env);
//        value_write(lamb->as.lambda.params);
//        printf("\n");
//        value_write(lamb->as.lambda.body);
//        printf("\n");
//        env_print(lamb->as.lambda.env);
//        return lamb;
        return value_make_lambda(cadr(exp), caddr(exp), env);
    } else if (strcmp(car(exp)->as.symbol, "cond") == 0) {
        return evcond(cdr(exp), env);
    } else if (strcmp(car(exp)->as.symbol, "define") == 0) {
        return env_define(cadr(exp), eval(caddr(exp), env), env);
    } else if (strcmp(car(exp)->as.symbol, "set!") == 0) {
        return env_update(cadr(exp), eval(caddr(exp), env), env);
    } else {
        return apply(eval(car(exp), env), evlist(cdr(exp), env));
    }
}

struct value*
apply(struct value* proc, struct value* args)
{
    if (proc->type == VALUE_BUILTIN) {
        // simply call the builtin
        return proc->as.builtin(args);
    } else if (proc->type == VALUE_LAMBDA) {
        // eval the lambda body in a new env that
        // binds the params to these args in a new frame
        // on top of the lambda's initial env
//        struct value* bound_env = env_bind(proc->as.lambda.params, args, proc->as.lambda.env);
//        env_print(bound_env);
//        return eval(proc->as.lambda.body, bound_env);
        return eval(proc->as.lambda.body, env_bind(proc->as.lambda.params, args, proc->as.lambda.env));
    } else {
        return value_make_error("unknown procedure type");
    }
}

struct value*
evlist(struct value* exps, struct value* env)
{
    if (exps == NULL) return NULL;

    return cons(eval(car(exps), env), evlist(cdr(exps), env));
}

struct value*
evcond(struct value* exps, struct value* env)
{
    if (exps == NULL) return NULL;

    if (caar(exps)->type == VALUE_SYMBOL &&
        strcmp(caar(exps)->as.symbol, "else") == 0) {
        return eval(cadar(exps), env);
    } else if (value_is_false(eval(caar(exps), env))) {
        return evcond(cdr(exps), env);
    } else {
        return eval(cadar(exps), env);
    }
}

int
//main(int argc, char* argv[])
main(void)
{
    struct value* vars = list_make(
        value_make_symbol("+", 1),
        value_make_symbol("*", 1),
        NULL);
    struct value* vals = list_make(
        value_make_builtin(builtin_plus),
        value_make_builtin(builtin_multiply),
        NULL);
    struct value* env = env_bind(vars, vals, NULL);

//    env_print(env);

    printf("> ");
    char line[512] = { 0 };
    while (fgets(line, sizeof(line), stdin) != NULL) {
        long consumed = 0;
        struct value* exp = value_read(line, &consumed);
//        write(exp);
//        printf("\n");

        struct value* res = eval(exp, env);
        value_write(res);
        printf("\n");

        printf("> ");
    }
}
