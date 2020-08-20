#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "list.h"
#include "mce.h"
#include "reader.h"
#include "value.h"
#include "vm.h"

// Meta-Circular Evaluator is based on SICP Chapter 4
// Helper macros are based on funcs from SICP 4.1.2

#define is_last_exp(exp)  \
  (value_is_empty_list(CDR(exp)))
#define first_exp(exp)  \
  CAR(exp)
#define rest_exps(exp)  \
  CDR(exp)

static struct value*
eval_sequence(struct vm* vm, struct value* exp, struct value* env)
{
    if (is_last_exp(exp)) {
        return mce_eval(vm, first_exp(exp), env);
    } else {
        mce_eval(vm, first_exp(exp), env);
        return eval_sequence(vm, rest_exps(exp), env);
    }
}

static struct value*
list_of_values(struct vm* vm, struct value* exps, struct value* env)
{
    if (value_is_empty_list(exps)) return exps;
    return vm_make_pair(vm, mce_eval(vm, first_exp(exps), env),
                            list_of_values(vm, rest_exps(exps), env));
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
eval_assignment(struct vm* vm, struct value* exp, struct value* env)
{
    return env_update(vm,
        assignment_var(exp),
        mce_eval(vm, assignment_val(exp), env),
        env);
}

// this looks like it creates an improper list but it doesn't
// because 'CDDR(exp)' will include the initial list's terminator
#define make_lambda(vm, exp)                      \
  vm_make_pair(vm, vm_make_symbol(vm, "lambda"),  \
                   vm_make_pair(vm, CDADR(exp), CDDR(exp)))

// 'define' supports two forms (the second is syntactic sugar for lambdas):
// NORMAL: (define square (lambda (x) (* x x)))
// SUGAR:  (define (square x) (* x x))

#define is_definition(exp)  \
  is_tagged_list(exp, "define")
#define definition_var(exp)  \
  value_is_symbol(CADR(exp)) \
  ? CADR(exp)                \
  : CAADR(exp)
#define definition_val(vm, exp)  \
  value_is_symbol(CADR(exp))     \
  ? CADDR(exp)                   \
  : make_lambda(vm, exp)

static struct value*
eval_definition(struct vm* vm, struct value* exp, struct value* env)
{
    // TODO: check for dot form
    // (define (foo . args) body) -> (define foo (lambda args body))
    return env_define(vm,
        definition_var(exp),
        mce_eval(vm, definition_val(vm, exp), env),
        env);
}

#define is_if(exp)  \
  is_tagged_list(exp, "if")
#define if_predicate(exp)  \
  CADR(exp)
#define if_consequent(exp)  \
  CADDR(exp)
#define if_alternative(vm, exp)  \
  (value_is_empty_list(CDDDR(exp)) ? vm_make_boolean(vm, false) : CADDDR(exp))

static struct value*
eval_if(struct vm* vm, struct value* exp, struct value* env)
{
    if (value_is_true(mce_eval(vm, if_predicate(exp), env))) {
        return if_consequent(exp);
    } else {
        return if_alternative(vm, exp);
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
load(struct vm* vm, struct value* args, struct value* env)
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
        struct value* exp = reader_read(vm, fp);
        if (value_is_eof(exp)) break;

        mce_eval(vm, exp, env);
    }

    fclose(fp);
    return vm_make_empty_list(vm);
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
mce_eval(struct vm* vm, struct value* exp, struct value* env)
{
tailcall:

    if (is_self_evaluating(exp)) {
        return exp;
    } else if (is_variable(exp)) {
        return env_lookup(vm, exp, env);
    } else if (is_quoted(exp)) {
        return text_of_quotation(exp);
    } else if (is_assignment(exp)) {
        return eval_assignment(vm, exp, env);
    } else if (is_definition(exp)) {
        return eval_definition(vm, exp, env);
    } else if (is_if(exp)) {
        exp = eval_if(vm, exp, env);
        goto tailcall;
    } else if (is_environment(exp)) {
        return env;
    } else if (is_load(exp)) {
        return load(vm, load_args(exp), env);
    } else if (is_lambda(exp)) {
        // TODO: check for the three different lambda forms:
        // (lambda (x) (* x x))
        // (lambda x x)
        // (lambda (x . rest) (append x rest))
        return vm_make_lambda(vm, lambda_params(exp), lambda_body(exp), env);
    } else if (is_application(exp)) {
        // 'apply' is evalutaed inline for TCO
        struct value* proc = mce_eval(vm, operator(exp), env);
        struct value* args = list_of_values(vm, operands(exp), env);

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
            return proc->as.builtin(vm, args);
        } else if (is_compound_proc(proc)) {
            // evaluate the lambda's body in the current stack frame (for TCO)
            env = env_extend(vm, proc->as.lambda.params, args, proc->as.lambda.env);
            exp = proc->as.lambda.body;
            while (!is_last_exp(exp)) {
                mce_eval(vm, first_exp(exp), env);
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
mce_apply(struct vm* vm, struct value* proc, struct value* args)
{
    if (is_primitive_proc(proc)) {
        return proc->as.builtin(vm, args);
    } else if (is_compound_proc(proc)) {
        return eval_sequence(vm,
            proc->as.lambda.body,
            env_extend(vm, proc->as.lambda.params, args, proc->as.lambda.env));
    } else {
        fprintf(stderr, "runtime error (invalid proc) at: TODO\n");
        exit(EXIT_FAILURE);
    }
}

struct value*
mce_builtin_eval(struct vm* vm, struct value* args)
{
    fprintf(stderr, "error: builtin 'eval' should be handled by the MCE\n");
    exit(EXIT_FAILURE);
}

struct value*
mce_builtin_apply(struct vm* vm, struct value* args)
{
    fprintf(stderr, "error: builtin 'apply' should be handled by the MCE\n");
    exit(EXIT_FAILURE);
}
