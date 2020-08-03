#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "mce.h"

static struct value*
evlist(struct value* exps, struct value* env)
{
    if (exps == NULL) return NULL;

    return cons(mce_eval(car(exps), env), evlist(cdr(exps), env));
}

static struct value*
evcond(struct value* exps, struct value* env)
{
    if (exps == NULL) return NULL;

    if (caar(exps)->type == VALUE_SYMBOL &&
        strcmp(caar(exps)->as.symbol, "else") == 0) {
        return mce_eval(cadar(exps), env);
    } else if (value_is_false(mce_eval(caar(exps), env))) {
        return evcond(cdr(exps), env);
    } else {
        return mce_eval(cadar(exps), env);
    }
}

struct value*
mce_eval(struct value* exp, struct value* env)
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
        return value_make_lambda(cadr(exp), caddr(exp), env);
    } else if (strcmp(car(exp)->as.symbol, "cond") == 0) {
        return evcond(cdr(exp), env);
    } else if (strcmp(car(exp)->as.symbol, "define") == 0) {
        return env_define(cadr(exp), mce_eval(caddr(exp), env), env);
    } else if (strcmp(car(exp)->as.symbol, "set!") == 0) {
        return env_update(cadr(exp), mce_eval(caddr(exp), env), env);
    } else {
        return mce_apply(mce_eval(car(exp), env), evlist(cdr(exp), env));
    }
}

struct value*
mce_apply(struct value* proc, struct value* args)
{
    if (proc->type == VALUE_BUILTIN) {
        // simply call the builtin
        return proc->as.builtin(args);
    } else if (proc->type == VALUE_LAMBDA) {
        // eval the lambda body in a new env that
        // binds the params to these args in a new frame
        // on top of the lambda's initial env
        return mce_eval(proc->as.lambda.body, env_bind(proc->as.lambda.params, args, proc->as.lambda.env));
    } else {
        return value_make_error("unknown procedure type");
    }
}
