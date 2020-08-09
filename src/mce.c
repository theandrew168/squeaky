#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "io.h"
#include "list.h"
#include "mce.h"
#include "value.h"

// Meta-Circular Evaluator is based on SICP Chapter 4
// Helpers are from SICP 4.1.2

static struct value*
list_of_values(struct value* exp, struct value* env)
{
    if (exp == NULL) return NULL;

    return cons(mce_eval(car(exp), env), list_of_values(cdr(exp), env));
}

static bool
is_tagged_list(struct value* exp, const char* tag)
{
    if (!value_is_pair(exp)) return false;
    return strcmp(car(exp)->as.symbol, tag) == 0;
}

#define is_self_evaluating(exp)  \
  value_is_boolean(exp) ||       \
  value_is_number(exp)  ||       \
  value_is_string(exp)

#define is_variable(exp)  \
  value_is_symbol(exp)

#define is_quoted(exp)  \
  is_tagged_list(exp, "quote")

static struct value*
text_of_quotation(struct value* exp)
{
    if (exp == EMPTY_LIST) return EMPTY_LIST;
    return cadr(exp);
}

#define is_assignment(exp)  \
  is_tagged_list(exp, "set!")
#define assignment_var(exp)  \
  cadr(exp)
#define assignment_val(exp)  \
  caddr(exp)

static struct value*
eval_assignment(struct value* exp, struct value* env)
{
    return env_update(
        assignment_var(exp),
        mce_eval(assignment_val(exp), env),
        env);
}

#define is_definition(exp)  \
  is_tagged_list(exp, "define")
#define definition_var(exp)  \
  value_is_symbol(cadr(exp)) \
  ? cadr(exp)                \
  : caadr(exp)
#define definition_val(exp)   \
  value_is_symbol(cadr(exp))  \
  ? caddr(exp)                \
  : cons(value_make_symbol("lambda"), cons(cdadr(exp), cddr(exp)))

static struct value*
eval_definition(struct value* exp, struct value* env)
{
    return env_define(
        definition_var(exp),
        mce_eval(definition_val(exp), env),
        env);
}

#define is_lambda(exp)  \
  is_tagged_list(exp, "lambda")
#define lambda_params(exp)  \
  cadr(exp)
#define lambda_body(exp)  \
  cddr(exp)

#define is_if(exp)  \
  is_tagged_list(exp, "if")
#define if_predicate(exp)  \
  cadr(exp)
#define if_consequent(exp)  \
  caddr(exp)
#define if_alternative(exp)  \
  ((cdddr(exp) == EMPTY_LIST) ? value_make_boolean(false) : cadddr(exp))

static struct value*
eval_if(struct value* exp, struct value* env)
{
    if (value_is_true(mce_eval(if_predicate(exp), env))) {
        return if_consequent(exp);
    } else {
        return if_alternative(exp);
    }
}

#define is_begin(exp)  \
  is_tagged_list(exp, "begin")
#define make_begin(exp)  \
  cons(value_make_symbol("begin"), exp)
#define begin_actions(exp)  \
  cdr(exp)
#define is_last_exp(exp)  \
  (cdr(exp) == NULL)
#define first_exp(exp)  \
  car(exp)
#define rest_exps(exp)  \
  cdr(exp)

#define is_load(exp)  \
  is_tagged_list(exp, "load")
#define load_path(exp)  \
  cadr(exp)

#define is_application(exp)  \
  value_is_pair(exp)

#define operator(exp)  \
  car(exp)
#define operands(exp)  \
  cdr(exp)

#define is_primitive_proc(exp)  \
  ((exp)->type == VALUE_BUILTIN)

#define is_compound_proc(exp)  \
  ((exp)->type == VALUE_LAMBDA)

struct value*
mce_eval(struct value* exp, struct value* env)
{
// R5RS mandates TCO on lambdas and the following exprs:
// if, cond, case, and, or, let, let*, letrec,
// let-syntax, letrec-syntax, begin, do
tailcall:
    if (exp == EMPTY_LIST) return EMPTY_LIST;

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
    } else if (is_lambda(exp)) {
        return value_make_lambda(lambda_params(exp), lambda_body(exp), env);
    } else if (is_begin(exp)) {
        exp = begin_actions(exp);
        while (!is_last_exp(exp)) {
            mce_eval(first_exp(exp), env);
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    } else if (is_application(exp)) {
        struct value* proc = mce_eval(operator(exp), env);
        struct value* args = list_of_values(operands(exp), env);

        // TODO: handle 'eval' builtin proc specifically
        // TODO: handle 'apply' builtin proc specifically

        if (is_primitive_proc(proc)) {
            // simply call the builtin
            return proc->as.builtin(args);
        } else if (is_compound_proc(proc)) {
            // evaluate the lambda's body in the current stack frame (for TCO) by
            // updating the current expression and current environment
            // - the new expression is the lambda's body wrapped in 'begin'
            // - the new environment contains bindings for the lambda's arguments
            exp = make_begin(proc->as.lambda.body);
            env = env_extend(proc->as.lambda.params, args, proc->as.lambda.env);
            goto tailcall;
        } else {
            fprintf(stderr, "runtime error (invalid proc) at: TODO\n");
            exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "runtime error (invalid expr) at: TODO\n");
    exit(EXIT_FAILURE);
}
