#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "io.h"
#include "list.h"
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

#define is_begin(exp)  \
  is_tagged_list(exp, "begin")
#define begin_actions(exp)  \
  cdr(exp)
#define is_last_exp(exp)  \
  (cdr(exp) == NULL)
#define first_exp(exp)  \
  car(exp)
#define rest_exps(exp)  \
  cdr(exp)

static struct value*
eval_sequence(struct value* exp, struct value* env)
{
    if (exp == NULL) return value_make_pair(NULL, NULL);

    if (is_last_exp(exp)) {
        return mce_eval(first_exp(exp), env);
    } else {
        mce_eval(first_exp(exp), env);
        return eval_sequence(rest_exps(exp), env);
    }
}

#define is_cond(exp)  \
  is_tagged_list(exp, "cond")

#define is_do(exp)  \
  is_tagged_list(exp, "do")
#define do_bindings(exp)  \
  cadr(exp)
#define do_cond(exp)  \
  caddr(exp)
#define do_command(exp)  \
  cdddr(exp)
#define do_cond_test(exp)  \
  car(exp)
#define do_cond_expr(exp)  \
  cdr(exp)
#define do_first_binding(exp)  \
  car(exp)
#define do_rest_bindings(exp)  \
  cdr(exp)
#define do_binding_var(exp)  \
  car(exp)
#define do_binding_init(exp)  \
  cadr(exp)
#define do_binding_step(exp)  \
  caddr(exp)
#define do_binding_has_step(exp)  \
  (cddr(exp) != NULL)

struct value*
eval_do(struct value* exp, struct value* env)
{
    // create a new frame
    struct value* do_env = env_extend(NULL, NULL, env);

    // init vars to their initial value
    for (struct value* bindings = do_bindings(exp);
         bindings != NULL;
         bindings = do_rest_bindings(bindings)) {
        struct value* binding = do_first_binding(bindings);
        env_define(do_binding_var(binding), do_binding_init(binding), do_env);
    }

    // loop until test condition is true
    struct value* cond = do_cond(exp);
    while (value_is_false(mce_eval(do_cond_test(cond), do_env))) {
        // eval the commands (could be none)
        eval_sequence(do_command(exp), do_env);

        // update vars based on their step expr (if present)
        for (struct value* bindings = do_bindings(exp);
             bindings != NULL;
             bindings = do_rest_bindings(bindings)) {

            // if step is present, eval it and update the value of the var
            struct value* binding = do_first_binding(bindings);
            if (do_binding_has_step(binding)) {
                env_update(
                    do_binding_var(binding),
                    mce_eval(do_binding_step(binding), do_env),
                    do_env);
            }
        }
    }

    // eval cond expressions (could be none) and return last
    return eval_sequence(do_cond_expr(cond), do_env);
}

#define is_let(exp)  \
  is_tagged_list(exp, "let")
#define is_let_star(exp)  \
  is_tagged_list(exp, "let*")
#define let_bindings(exp)  \
  cadr(exp)
#define let_body(exp)  \
  cddr(exp)
#define let_first_binding(exp)  \
  car(exp)
#define let_rest_bindings(exp)  \
  cdr(exp)
#define let_binding_var(exp)  \
  car(exp)
#define let_binding_val(exp)  \
  cadr(exp)

struct value*
eval_let(struct value* exp, struct value* env)
{
    struct value* let_env = env_extend(NULL, NULL, env);
    for (struct value* bindings = let_bindings(exp);
         bindings != NULL;
         bindings = let_rest_bindings(bindings)) {
        struct value* binding = let_first_binding(bindings);
        env_define(let_binding_var(binding), mce_eval(let_binding_val(binding), env), let_env);
    }
    return eval_sequence(let_body(exp), let_env);
}

struct value*
eval_let_star(struct value* exp, struct value* env)
{
    struct value* let_env = env_extend(NULL, NULL, env);
    for (struct value* bindings = let_bindings(exp);
         bindings != NULL;
         bindings = let_rest_bindings(bindings)) {
        struct value* binding = let_first_binding(bindings);
        env_define(let_binding_var(binding), mce_eval(let_binding_val(binding), let_env), let_env);
    }
    return eval_sequence(let_body(exp), let_env);
}

#define is_and(exp)  \
  is_tagged_list(exp, "and")
#define and_tests(exp)  \
  cdr(exp)

#define is_or(exp)  \
  is_tagged_list(exp, "or")
#define or_tests(exp)  \
  cdr(exp)

#define is_application(exp)  \
  value_is_pair(exp)

#define is_load(exp)  \
  is_tagged_list(exp, "load")
#define load_path(exp)  \
  cadr(exp)

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
    if (exp == EMPTY_LIST) return value_make_error("unknown expression type!");

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
        exp = value_is_true(mce_eval(if_predicate(exp), env))
            ? if_consequent(exp)
            : if_alternative(exp);
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
    } else if (is_cond(exp)) {
        exp = eval_cond(cdr(exp), env);
        goto tailcall;
    } else if (is_do(exp)) {
        exp = eval_do(exp, env);
        goto tailcall;
    } else if (is_let(exp)) {
        exp = eval_let(exp, env);
        goto tailcall;
    } else if (is_let_star(exp)) {
        exp = eval_let_star(exp, env);
        goto tailcall;
    } else if (is_and(exp)) {
        exp = and_tests(exp);
        if (exp == NULL) return value_make_boolean(true);
        while (!is_last_exp(exp)) {
            struct value* res = mce_eval(first_exp(exp), env);
            if (value_is_false(res)) return res;
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    } else if (is_or(exp)) {
        exp = or_tests(exp);
        if (exp == NULL) return value_make_boolean(false);
        while (!is_last_exp(exp)) {
            struct value* res = mce_eval(first_exp(exp), env);
            if (value_is_true(res)) return res;
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    } else if (is_load(exp)) {
        return mce_load(load_path(exp), env);
    } else if (is_application(exp)) {
        struct value* proc = mce_eval(car(exp), env);
        struct value* args = eval_list(cdr(exp), env);

        // TODO: handle 'eval' builtin specifically
        // TODO: handle 'apply' builtin specifically

        if (is_primitive_proc(proc)) {
            // simply call the builtin
            return proc->as.builtin(args);
//            struct value* res = proc->as.builtin(args);
//            value_ref_dec(args);
//            return res;
        } else if (is_compound_proc(proc)) {
            // update the env to includes bound args and the lambda's closured env
            // update the exp to be the lambda's body wrapped in a 'begin'
            env = env_extend(proc->as.lambda.params, args, proc->as.lambda.env);
            exp = cons(value_make_symbol("begin"), proc->as.lambda.body);
            goto tailcall;
        } else {
            return value_make_error("unknown procedure type");
        }
    }

    return value_make_error("unknown expression type!");
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
        struct value* exp = io_read(source + total, &consumed);
        total += consumed;

        // check for reader errors
        if (value_is_error(exp)) {
            // trailing newlines on unix text files might finish with an empty expr
            if (strcmp(exp->as.error, "EOF") == 0) {
                break;
            } else {
                return exp;
            }
        }

        // if not empty, then eval the expr and check for errors
        struct value* res = mce_eval(exp, env);
        if (value_is_error(res)) {
            return res;
        }
    }

    // free the big string
    free(source);
    return EMPTY_LIST;
}
