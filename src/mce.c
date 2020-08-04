#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "mce.h"

// Meta-Circular Evaluator is based on SICP Chapter 4

static struct value*
eval_list(struct value* exp, struct value* env)
{
    if (exp == NULL) return NULL;

    return cons(mce_eval(car(exp), env), eval_list(cdr(exp), env));
}

static struct value*
eval_cond(struct value* exp, struct value* env)
{
    if (exp == NULL) return NULL;

    if (caar(exp)->type == VALUE_SYMBOL &&
        strcmp(caar(exp)->as.symbol, "else") == 0) {
        return mce_eval(cadar(exp), env);
    } else if (value_is_false(mce_eval(caar(exp), env))) {
        return eval_cond(cdr(exp), env);
    } else {
        return mce_eval(cadar(exp), env);
    }
}

// Helpers from SICP 4.1.2

static bool
is_tagged_list(struct value* exp, const char* tag)
{
    if (!value_is_pair(exp)) return false;
    return strcmp(car(exp)->as.symbol, tag) == 0;
}

#define is_self_evaluating(exp)  \
  value_is_boolean(exp)          \
  || value_is_number(exp)        \
  || value_is_string(exp)        \
  || value_is_error(exp)

#define is_variable(exp)  \
  value_is_symbol(exp)

#define is_quoted(exp)  \
  is_tagged_list(exp, "quote")
#define text_of_quotation(exp)  \
  cadr(exp)

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
  caddr(exp)

#define is_if(exp)  \
  is_tagged_list(exp, "if")
#define if_predicate(exp)  \
  cadr(exp)
#define if_consequent(exp)  \
  caddr(exp)
#define if_alternative(exp)  \
  !list_is_null(cdddr(exp)) ? cadddr(exp) : value_make_boolean(false)

static struct value*
eval_if(struct value* exp, struct value* env)
{
    if (value_is_true(mce_eval(if_predicate(exp), env))) {
        return mce_eval(if_consequent(exp), env);
    } else {
        return mce_eval(if_alternative(exp), env);
    }
}

#define is_begin(exp)  \
  is_tagged_list(exp, "begin")
#define begin_actions(exp)  \
  cdr(exp)
#define is_last_exp(exp)  \
  list_is_null(cdr(exp))
#define first_exp(exp)  \
  car(exp)
#define rest_exps(exp)  \
  cdr(exp)

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

#define is_cond(exp)  \
  is_tagged_list(exp, "cond")

#define is_application(exp)  \
  value_is_pair(exp)

#define is_load(exp)  \
  is_tagged_list(exp, "load")
#define load_path(exp)  \
  cadr(exp)

struct value*
mce_eval(struct value* exp, struct value* env)
{
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
        return eval_if(exp, env);
    } else if (is_lambda(exp)) {
        return value_make_lambda(lambda_params(exp), lambda_body(exp), env);
    } else if (is_begin(exp)) {
        return eval_sequence(begin_actions(exp), env);
    } else if (is_cond(exp)) {
        return eval_cond(cdr(exp), env);
    } else if (is_load(exp)) {
        return mce_load(load_path(exp), env);
    } else if (is_application(exp)) {
        return mce_apply(mce_eval(car(exp), env), eval_list(cdr(exp), env));
    }

    return value_make_error("unknown expression type!");
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

struct value*
mce_load(struct value* args, struct value* env)
{
    assert(args != NULL);
    assert(env != NULL);
    // TODO assert 1 arg (string)

    const char* path = args->as.string;

    // open the file
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "failed to open file: %s\n", path);
        return value_make_error("failed to open file");
    }

    // find out how big the file is
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // read the file into a big string
    char* source = malloc(size + 1);
    fread(source, 1, size, file);
    source[size] = '\0';

    // close the file
    fclose(file);

    // iteratively read and eval the file's expressions
    long total = 0;
    while (total != size) {
        // keep track of how many bytes have been read / evaluated
        long consumed = 0;
        struct value* exp = value_read(source + total, &consumed);
        total += consumed;

        // trailing newlines on unix text files might finish with an empty expr
        if (list_is_null(exp)) break;

        // if not empty, then eval the expr and check for errors
        struct value* res = mce_eval(exp, env);
        if (value_is_error(res)) {
            return res;
        }
    }

    // free the big string
    free(source);
    return value_make_pair(NULL, NULL);
}
