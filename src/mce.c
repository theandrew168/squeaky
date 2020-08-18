#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "list.h"
#include "mce.h"
#include "reader.h"
#include "value.h"

// Meta-Circular Evaluator is based on SICP Chapter 4
// Helper macros are based on funcs from SICP 4.1.2

#define is_last_exp(exp)  \
  (value_is_empty_list(CDR(exp)))
#define first_exp(exp)  \
  CAR(exp)
#define rest_exps(exp)  \
  CDR(exp)

static struct value*
eval_sequence(struct value* exp, struct value* env)
{
    if (is_last_exp(exp)) {
        return mce_eval(first_exp(exp), env);
    } else {
        mce_eval(first_exp(exp), env);
        return eval_sequence(rest_exps(exp), env);
    }
}

static struct value*
list_of_values(struct value* exps, struct value* env)
{
    if (value_is_empty_list(exps)) return exps;
    return CONS(mce_eval(first_exp(exps), env),
                list_of_values(rest_exps(exps), env));
}

static bool
is_tagged_list(struct value* exp, const char* tag)
{
    if (!value_is_pair(exp)) return false;
    return strcmp(CAR(exp)->as.symbol, tag) == 0;
}

#define is_self_evaluating(exp)  \
  value_is_boolean(exp)          \
  || value_is_character(exp)     \
  || value_is_number(exp)        \
  || value_is_string(exp)

#define is_variable(exp)  \
  value_is_symbol(exp)

#define is_quoted(exp)  \
  is_tagged_list(exp, "quote")

static struct value*
text_of_quotation(struct value* exp)
{
    if (value_is_empty_list(exp)) return exp;
    return CADR(exp);
}

#define is_assignment(exp)  \
  is_tagged_list(exp, "set!")
#define assignment_var(exp)  \
  CADR(exp)
#define assignment_val(exp)  \
  CADDR(exp)

static struct value*
eval_assignment(struct value* exp, struct value* env)
{
    return env_update(
        assignment_var(exp),
        mce_eval(assignment_val(exp), env),
        env);
}

// this looks like it creates an improper list but it doesn't
// because 'CDDR(exp)' will include the initial list's terminator
#define make_lambda(exp)  \
  CONS(value_make_symbol("lambda"), CONS(CDADR(exp), CDDR(exp)))

// 'define' supports two forms (the second is syntactic sugar for lambdas):
// NORMAL: (define square (lambda (x) (* x x)))
// SUGAR:  (define (square x) (* x x))

#define is_definition(exp)  \
  is_tagged_list(exp, "define")
#define definition_var(exp)  \
  value_is_symbol(CADR(exp)) \
  ? CADR(exp)                \
  : CAADR(exp)
#define definition_val(exp)   \
  value_is_symbol(CADR(exp))  \
  ? CADDR(exp)                \
  : make_lambda(exp)

static struct value*
eval_definition(struct value* exp, struct value* env)
{
    // TODO: check for dot form
    // (define (foo . args) body) -> (define foo (lambda args body))
    return env_define(
        definition_var(exp),
        mce_eval(definition_val(exp), env),
        env);
}

#define is_if(exp)  \
  is_tagged_list(exp, "if")
#define if_predicate(exp)  \
  CADR(exp)
#define if_consequent(exp)  \
  CADDR(exp)
#define if_alternative(exp)  \
  (value_is_empty_list(CDDDR(exp)) ? value_make_boolean(false) : CADDDR(exp))

static struct value*
eval_if(struct value* exp, struct value* env)
{
    if (value_is_true(mce_eval(if_predicate(exp), env))) {
        return if_consequent(exp);
    } else {
        return if_alternative(exp);
    }
}

#define is_environment(exp)                          \
  is_tagged_list(exp, "scheme-report-environment")   \
  || is_tagged_list(exp, "null-environment")         \
  || is_tagged_list(exp, "interaction-environment")
#define environment_version(exp)  \
  CADR(exp)

#define is_load(exp)  \
  is_tagged_list(exp, "load")
#define load_args(exp)  \
  CDR(exp)

static struct value*
load(struct value* args, struct value* env)
{
    ASSERT_ARITY("load", args, 1);
    ASSERT_TYPE("load", args, 0, VALUE_STRING);

    struct value* path = list_nth(args, 0);

    FILE* fp = fopen(path->as.string, "rb");
    if (fp == NULL) {
        fprintf(stderr, "failed to load file: %s\n", path->as.string);
        exit(EXIT_FAILURE);
    }

    while (!feof(fp)) {
        struct value* exp = reader_read(fp);
        if (value_is_eof(exp)) break;

        mce_eval(exp, env);
    }

    fclose(fp);
    return value_make_empty_list();
}

#define is_lambda(exp)  \
  is_tagged_list(exp, "lambda")
#define lambda_params(exp)  \
  CADR(exp)
#define lambda_body(exp)  \
  CDDR(exp)

#define is_application(exp)  \
  value_is_pair(exp)

#define operator(exp)  \
  CAR(exp)
#define operands(exp)  \
  CDR(exp)

#define is_primitive_proc(exp)  \
  ((exp)->type == VALUE_BUILTIN)

#define is_compound_proc(exp)  \
  ((exp)->type == VALUE_LAMBDA)

#define eval_exp(exp)  \
  CAR(exp)
#define eval_env(exp)  \
  CADR(exp)

#define apply_operator(exp)  \
  CAR(exp)
#define apply_operands(exp)  \
  CDR(exp)

struct value*
mce_eval(struct value* exp, struct value* env)
{
tailcall:

    if (is_self_evaluating(exp)) {
        return exp;
    } else if (is_variable(exp)) {
        return env_lookup(exp, env);
    } else if (is_quoted(exp)) {
        return text_of_quotation(exp);
    } else if (is_assignment(exp)) {
        return eval_assignment(exp, env);
    } else if (is_definition(exp)) {
        return eval_definition(exp, env);
    } else if (is_if(exp)) {
        exp = eval_if(exp, env);
        goto tailcall;
    } else if (is_environment(exp)) {
        return env;
    } else if (is_load(exp)) {
        return load(load_args(exp), env);
    } else if (is_lambda(exp)) {
        // TODO: check for the three different lambda forms:
        // (lambda (x) (* x x))
        // (lambda x x)
        // (lambda (x . rest) (append x rest))
        return value_make_lambda(lambda_params(exp), lambda_body(exp), env);
    } else if (is_application(exp)) {
        // 'apply' is evalutaed inline for TCO
        struct value* proc = mce_eval(operator(exp), env);
        struct value* args = list_of_values(operands(exp), env);

        // handle builtin 'eval' specifically for TCO
        if (is_primitive_proc(proc) && proc->as.builtin == mce_builtin_eval) {
            ASSERT_ARITY("eval", args, 2);
            ASSERT_TYPE("eval", args, 1, VALUE_PAIR);
            env = eval_env(args);
            exp = eval_exp(args);
            goto tailcall;
        }

        // handle builtin 'apply' specifically for TCO
        if (is_primitive_proc(proc) && proc->as.builtin == mce_builtin_apply) {
            proc = apply_operator(args);
            args = apply_operands(args);
        }

        if (is_primitive_proc(proc)) {
            return proc->as.builtin(args);
        } else if (is_compound_proc(proc)) {
            // evaluate the lambda's body in the current stack frame (for TCO)
            env = env_extend(proc->as.lambda.params, args, proc->as.lambda.env);
            exp = proc->as.lambda.body;
            while (!is_last_exp(exp)) {
                mce_eval(first_exp(exp), env);
                exp = rest_exps(exp);
            }
            exp = first_exp(exp);
            goto tailcall;
        } else {
            fprintf(stderr, "runtime error (invalid proc) at: TODO\n");
            exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "runtime error (invalid expr) at: TODO\n");
    exit(EXIT_FAILURE);
}

struct value*
mce_apply(struct value* proc, struct value* args)
{
    if (is_primitive_proc(proc)) {
        return proc->as.builtin(args);
    } else if (is_compound_proc(proc)) {
        return eval_sequence(
            proc->as.lambda.body,
            env_extend(proc->as.lambda.params, args, proc->as.lambda.env));
    } else {
        fprintf(stderr, "runtime error (invalid proc) at: TODO\n");
        exit(EXIT_FAILURE);
    }
}

struct value*
mce_builtin_eval(struct value* args)
{
    fprintf(stderr, "error: builtin 'eval' should be handled by the MCE\n");
    exit(EXIT_FAILURE);
}

struct value*
mce_builtin_apply(struct value* args)
{
    fprintf(stderr, "error: builtin 'apply' should be handled by the MCE\n");
    exit(EXIT_FAILURE);
}
