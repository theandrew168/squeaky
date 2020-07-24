#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "lenv.h"
#include "lval.h"

union lval* eval_sexpr(struct lenv* env, union lval* val);
union lval* eval(struct lenv* env, union lval* val);

union lval*
builtin_op(struct lenv* env, union lval* val, char* op)
{
    struct lval_qexpr* v = AS_QEXPR(val);

    // ensure all args are numbers
    for (long i = 0; i < v->count; i++) {
        LASSERT_TYPE(op, val, i, LVAL_TYPE_NUMBER);
    }

    // pop the first element
    union lval* x = lval_list_pop(val, 0);

    // if no args and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && v->count == 0) {
        AS_NUMBER(x)->number = -AS_NUMBER(x)->number;
    }

    // mathematically condense all children into x
    while (v->count > 0) {
        union lval* y = lval_list_pop(val, 0);

        if (strcmp(op, "+") == 0) x->number += y->number;
        if (strcmp(op, "-") == 0) x->number -= y->number;
        if (strcmp(op, "*") == 0) x->number *= y->number;
        if (strcmp(op, "/") == 0) {
            if (y->number == 0) {
                lval_free(x);
                lval_free(y);
                x = lval_make_error("division by zero");
                break;
            }
            x->number /= y->number;
        }

        lval_free(y);
    }

    // free the original list and return the calculated x
    lval_free(val);
    return x;
}

union lval*
builtin_add(struct lenv* env, union lval* val)
{
    return builtin_op(env, val, "+");
}

union lval*
builtin_sub(struct lenv* env, union lval* val)
{
    return builtin_op(env, val, "-");
}

union lval*
builtin_mul(struct lenv* env, union lval* val)
{
    return builtin_op(env, val, "*");
}

union lval*
builtin_div(struct lenv* env, union lval* val)
{
    return builtin_op(env, val, "/");
}

union lval*
builtin_ord(struct lenv* env, union lval* val, const char* op)
{
    LASSERT_ARITY(op, val, 2);
    LASSERT_TYPE(op, val, 0, LVAL_TYPE_NUMBER);
    LASSERT_TYPE(op, val, 1, LVAL_TYPE_NUMBER);
    
    long res = -1;
    if (strcmp(op, "<") == 0) res = val->list[0]->number < val->list[1]->number ? 1 : 0;
    if (strcmp(op, "<=") == 0) res = val->list[0]->number <= val->list[1]->number ? 1 : 0;
    if (strcmp(op, ">") == 0) res = val->list[0]->number > val->list[1]->number ? 1 : 0;
    if (strcmp(op, ">=") == 0) res = val->list[0]->number >= val->list[1]->number ? 1 : 0;

    lval_free(val);
    return lval_make_number(res);
}

union lval*
builtin_lt(struct lenv* env, union lval* val)
{
    return builtin_ord(env, val, "<");
}

union lval*
builtin_lte(struct lenv* env, union lval* val)
{
    return builtin_ord(env, val, "<=");
}

union lval*
builtin_gt(struct lenv* env, union lval* val)
{
    return builtin_ord(env, val, ">");
}

union lval*
builtin_gte(struct lenv* env, union lval* val)
{
    return builtin_ord(env, val, ">=");
}

union lval*
builtin_cmp(struct lenv* env, union lval* val, const char* op)
{
    LASSERT_ARITY(op, val, 2);

    long res = -1;
    if (strcmp(op, "==") == 0) res = lval_eq(val->list[0], val->list[1]);
    if (strcmp(op, "!=") == 0) res = !lval_eq(val->list[0], val->list[1]);

    lval_free(val);
    return lval_make_number(res);
}

union lval*
builtin_eq(struct lenv* env, union lval* val)
{
    return builtin_cmp(env, val, "==");
}

union lval*
builtin_neq(struct lenv* env, union lval* val)
{
    return builtin_cmp(env, val, "!=");
}

union lval*
builtin_if(struct lenv* env, union lval* val)
{
    LASSERT_ARITY("if", val, 3);
    LASSERT_TYPE("if", val, 0, LVAL_TYPE_NUMBER);
    LASSERT_TYPE("if", val, 1, LVAL_TYPE_QEXPR);
    LASSERT_TYPE("if", val, 2, LVAL_TYPE_QEXPR);

    // mark both expressions as evaluable
    val->list[1]->type = LVAL_TYPE_SEXPR;
    val->list[2]->type = LVAL_TYPE_SEXPR;

    union lval* res = NULL;
    if (val->list[0]->number) {
        // if condition is true then eval the first expression
        res = eval(env, lval_list_pop(val, 1));
    } else {
        // else eval the second expression
        res = eval(env, lval_list_pop(val, 2));
    }

    lval_free(val);
    return res;
}

union lval*
builtin_load(struct lenv* env, union lval* val)
{
    LASSERT_ARITY("load", val, 1);
    LASSERT_TYPE("load", val, 0, LVAL_TYPE_STRING);

    FILE* f = fopen(val->list[0]->string, "rb");
    if (f == NULL) {
        union lval* error = lval_make_error("load failed: %s", val->list[0]->string);
        lval_free(val);
        return error;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* input = malloc(size + 1);
    fread(input, 1, size, f);
    input[size] = '\0';
    fclose(f);

    long pos = 0;
    union lval* expr = lval_read_expr(input, &pos, '\0');
    free(input);

    if (expr->type != LVAL_TYPE_ERROR) {
        while (expr->count > 0) {
            union lval* x = eval(env, lval_list_pop(expr, 0));
            if (x->type == LVAL_TYPE_ERROR) lval_println(x);
            lval_free(x);
        }
    } else {
        lval_println(expr);
    }

    lval_free(expr);
    lval_free(val);

    return lval_make_sexpr();
}

union lval*
builtin_display(struct lenv* env, union lval* val)
{
    for (long i = 0; i < val->count; i++) {
        lval_print(val->list[i]);
        putchar(' ');
    }

    lval_free(val);

    return lval_make_sexpr();
}

union lval*
builtin_newline(struct lenv* env, union lval* val)
{
    putchar('\n');
    lval_free(val);

    return lval_make_sexpr();
}

union lval*
builtin_error(struct lenv* env, union lval* val)
{
    LASSERT_ARITY("error", val, 1);
    LASSERT_TYPE("error", val, 0, LVAL_TYPE_STRING);

    union lval* error = lval_make_error(val->list[0]->string);

    lval_free(val);
    return error;
}

union lval*
builtin_list(struct lenv* env, union lval* val)
{
    val->type = LVAL_TYPE_QEXPR;
    return val;
}

union lval*
builtin_head(struct lenv* env, union lval* val)
{
    LASSERT_ARITY("head", val, 1);
    LASSERT_TYPE("head", val, 0, LVAL_TYPE_QEXPR);
    LASSERT_NOT_EMPTY("head", val, 0);

    union lval* head = lval_list_take(val, 0);
    while (head->count > 1) lval_free(lval_list_pop(head, 1));
    return head;
}

union lval*
builtin_tail(struct lenv* env, union lval* val)
{
    LASSERT_ARITY("tail", val, 1);
    LASSERT_TYPE("tail", val, 0, LVAL_TYPE_QEXPR);
    LASSERT_NOT_EMPTY("tail", val, 0);

    union lval* tail = lval_list_take(val, 0);
    lval_free(lval_list_pop(tail, 0));
    return tail;
}

union lval*
builtin_join(struct lenv* env, union lval* val)
{
    for (long i = 0; i < val->count; i++ ) {
        LASSERT_TYPE("join", val, i, LVAL_TYPE_QEXPR);
    }

    union lval* list = lval_list_pop(val, 0);
    while (val->count) {
        list = lval_list_join(list, lval_list_pop(val, 0));
    }
    lval_free(val);

    return list;
}

union lval*
builtin_eval(struct lenv* env, union lval* val)
{
    LASSERT_ARITY("eval", val, 1);
    LASSERT_TYPE("eval", val, 0, LVAL_TYPE_QEXPR);

    union lval* sexpr = lval_list_take(val, 0);
    sexpr->type = LVAL_TYPE_SEXPR;
    return eval(env, sexpr);
}

union lval*
builtin_var(struct lenv* env, union lval* val, const char* func)
{
    LASSERT_TYPE(func, val, 0, LVAL_TYPE_QEXPR);

    union lval* symbols = val->list[0];
    for (long i = 0; i < symbols->count; i++) {
        LASSERTF(val, symbols->list[i]->type == LVAL_TYPE_SYMBOL,
            "function '%s' cannot define non-symbol: "
            "got %s at index %i",
            func, lval_type_name(symbols->list[i]->type), i);
    }

    LASSERTF(val, symbols->count == val->count - 1,
        "function '%s' passed mismatched number of symbols and values: "
        "want %i, got %i",
        func, val->count - 1, symbols->count);

    // assign copies of valects to symbols
    for (long i = 0; i < symbols->count; i++) {
        if (strcmp(func, "def") == 0) {
            lenv_def(env, symbols->list[i], val->list[i + 1]);
        }
        if (strcmp(func, "=") == 0) {
            lenv_put(env, symbols->list[i], val->list[i + 1]);
        }
    }

    lval_free(val);
    return lval_make_sexpr();
}

union lval*
builtin_def(struct lenv* env, union lval* val)
{
    return builtin_var(env, val, "def");
}

union lval*
builtin_put(struct lenv* env, union lval* val)
{
    return builtin_var(env, val, "=");
}

union lval*
builtin_lambda(struct lenv* env, union lval* val)
{
    LASSERT_ARITY("lambda", val, 2);
    LASSERT_TYPE("lambda", val, 0, LVAL_TYPE_QEXPR);
    LASSERT_TYPE("lambda", val, 1, LVAL_TYPE_QEXPR);

    // ensure first qexpr contains only symbols
    for (long i = 0; i < val->list[0]->count; i++) {
        LASSERT(val, val->list[0]->list[i]->type == LVAL_TYPE_SYMBOL, "function 'lambda' cannot define non-symbol");
    }

    union lval* formals = lval_list_pop(val, 0);
    union lval* body = lval_list_pop(val, 0);
    lval_free(val);

    return lval_make_lambda(formals, body);
}

union lval*
builtin_make_window(struct lenv* env, union lval* val)
{
    LASSERT_ARITY("make-window", val, 3);
    LASSERT_TYPE("make-window", val, 0, LVAL_TYPE_STRING);
    LASSERT_TYPE("make-window", val, 1, LVAL_TYPE_NUMBER);
    LASSERT_TYPE("make-window", val, 2, LVAL_TYPE_NUMBER);

    const char* title = val->list[0]->string;
    long width = val->list[1]->number;
    long height = val->list[2]->number;

    union lval* window = lval_make_window(title, width, height);
    lval_free(val);
    return window;
}

union lval*
call(struct lenv* env, union lval* func, union lval* val)
{
    // if builtin then just call it
    if (func->builtin != NULL) {
        return func->builtin(env, val);
    }

    long given = val->count;
    long total = func->formals->count;

    while (val->count > 0) {
        LASSERTF(val, func->formals->count > 0,
            "function passed too many args: want %i, got %i",
            total, given);

        // match next symbol with next value and add to env
        union lval* symbol = lval_list_pop(func->formals, 0);

        // special case to deal with '&'
        if (strcmp(symbol->symbol, "&") == 0) {
            // ensure '&' is followed by a symbol
            if (func->formals->count != 1) {
                lval_free(val);
                return lval_make_error("invalid function format: symbol '&' not followed by single symbol");
            }

            // bind next formal to a list of the remaining args
            union lval* group = lval_list_pop(func->formals, 0);
            lenv_put(func->env, group, builtin_list(env, val));
            lval_free(symbol);
            lval_free(group);
            break;
        }

        union lval* value = lval_list_pop(val, 0);
        lenv_put(func->env, symbol, value);
        lval_free(symbol);
        lval_free(value);
    }

    // args are all bound so orig list can be freed
    lval_free(val);

    // if '&' remains in formals then bind it to an empty list
    if (func->formals->count > 0 && strcmp(func->formals->list[0]->symbol, "&") == 0) {
        // ensure '&' is followed by a symbol
        if (func->formals->count != 2) {
            return lval_make_error("invalid function format: symbol '&' not followed by single symbol");
        }

        // pop and delete '&' symbol
        lval_free(lval_list_pop(func->formals, 0));

        // pop next symbol and create empty list
        union lval* symbol = lval_list_pop(func->formals, 0);
        union lval* value = lval_make_qexpr();

        // bind to env and delete
        lenv_put(func->env, symbol, value);
        lval_free(symbol);
        lval_free(value);
    }

    // eval if all args have been bound
    if (func->formals->count == 0) {
        // set the parent env and eval the body
        func->env->parent = env;
        return builtin_eval(func->env, lval_list_append(lval_make_sexpr(), lval_copy(func->body)));
    } else {
        // otherwise return the partially evaluated function
        return lval_copy(func);
    }
}

union lval*
eval_sexpr(struct lenv* env, union lval* val)
{
    // eval children
    for (long i = 0; i < val->count; i++) {
        val->list[i] = eval(env, val->list[i]);
    }

    // error checking
    for (long i = 0; i < val->count; i++) {
        if (val->list[i]->type == LVAL_TYPE_ERROR) return lval_list_take(val, i);
    }

    // empty expression
    if (val->count == 0) return val;

    // single expression
//    if (val->count == 1) return lval_list_take(val, 0);

    // ensure first element is a symbol
    union lval* func = lval_list_pop(val, 0);
    if (func->type != LVAL_TYPE_FUNC) {
        lval_free(func);
        lval_free(val);
        return lval_make_error(
            "s-expressions starts with invalid type: want %s, got %s",
            lval_type_name(LVAL_TYPE_FUNC), lval_type_name(func->type)); 
    }

    // call builtin with operator
    union lval* result = call(env, func, val);
    lval_free(func);
    return result;
}

union lval*
eval(struct lenv* env, union lval* val)
{
    if (val->type == LVAL_TYPE_SYMBOL) {
        union lval* v = lenv_get(env, val);
        lval_free(val);
        return v;
    }

    if (val->type == LVAL_TYPE_SEXPR) {
        return eval_sexpr(env, val);
    }

    return val;
}

void
add_builtin(struct lenv* env, const char* name, lbuiltin func)
{
    union lval* k = lval_make_symbol(name);
    union lval* v = lval_make_builtin(func);
    lenv_put(env, k, v);
    lval_free(k);
    lval_free(v);
}

void
add_builtins(struct lenv* env)
{
    add_builtin(env, "list", builtin_list);
    add_builtin(env, "head", builtin_head);
    add_builtin(env, "tail", builtin_tail);
    add_builtin(env, "join", builtin_join);
    add_builtin(env, "eval", builtin_eval);

    add_builtin(env, "+", builtin_add);
    add_builtin(env, "-", builtin_sub);
    add_builtin(env, "*", builtin_mul);
    add_builtin(env, "/", builtin_div);
    add_builtin(env, "<", builtin_lt);
    add_builtin(env, "<=", builtin_lte);
    add_builtin(env, ">", builtin_gt);
    add_builtin(env, ">=", builtin_gte);
    add_builtin(env, "==", builtin_eq);
    add_builtin(env, "!=", builtin_neq);
    add_builtin(env, "if", builtin_if);

    add_builtin(env, "load", builtin_load);
    add_builtin(env, "display", builtin_display);
    add_builtin(env, "newline", builtin_newline);
    add_builtin(env, "error", builtin_error);

    add_builtin(env, "def", builtin_def);
    add_builtin(env, "=", builtin_put);
    add_builtin(env, "lambda", builtin_lambda);

    add_builtin(env, "make-window", builtin_make_window);
}

int
main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to init SDL2: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    struct lenv* env = lenv_make();
    add_builtins(env);

    // silently attempt to load prelude
    union lval* res = builtin_load(env,
        lval_list_append(lval_make_sexpr(), lval_make_string("prelude.sqky")));
    lval_free(res);

    if (argc >= 2) {
        for (long i = 1; i < argc; i++) {
            union lval* args = lval_list_append(lval_make_sexpr(), lval_make_string(argv[i]));
            union lval* x = builtin_load(env, args);
            if (x->type == LVAL_TYPE_ERROR) lval_println(x);
            lval_free(x);
        }
    } else {
        puts("Welcome to Squeaky Scheme!");
        puts("Use Ctrl-c to exit.");

        printf("squeaky> ");

        char line[512] = { 0 };
        while (fgets(line, sizeof(line), stdin) != NULL) {
            long pos = 0;
            union lval* expr = lval_read_expr(line, &pos, '\0');
            while (expr->count > 0) {
                union lval* res = eval(env, lval_list_pop(expr, 0));
                if (res->type == LVAL_TYPE_SEXPR && res->count == 0) {
                    // don't print empty () responses
                } else {
                    lval_println(res);
                }
                lval_free(res);
            }

            lval_free(expr);

            printf("squeaky> ");
        }
    }

    lenv_free(env);
    SDL_Quit();
    return EXIT_SUCCESS;
}
