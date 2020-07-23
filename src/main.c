#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "lenv.h"
#include "lval.h"

struct lval* eval_sexpr(struct lenv* env, struct lval* val);
struct lval* eval(struct lenv* env, struct lval* val);

struct lval*
builtin_op(struct lenv* env, struct lval* val, char* op)
{
    // ensure all args are numbers
    for (long i = 0; i < val->cell_count; i++) {
        LASSERT_TYPE(op, val, i, LVAL_TYPE_NUMBER);
    }

    // pop the first element
    struct lval* x = lval_list_pop(val, 0);

    // if no args and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && val->cell_count == 0) {
        x->number = -x->number;
    }

    // mathematically condense all children into x
    while (val->cell_count > 0) {
        struct lval* y = lval_list_pop(val, 0);

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

struct lval*
builtin_add(struct lenv* env, struct lval* val)
{
    return builtin_op(env, val, "+");
}

struct lval*
builtin_sub(struct lenv* env, struct lval* val)
{
    return builtin_op(env, val, "-");
}

struct lval*
builtin_mul(struct lenv* env, struct lval* val)
{
    return builtin_op(env, val, "*");
}

struct lval*
builtin_div(struct lenv* env, struct lval* val)
{
    return builtin_op(env, val, "/");
}

struct lval*
builtin_ord(struct lenv* env, struct lval* val, const char* op)
{
    LASSERT_ARITY(op, val, 2);
    LASSERT_TYPE(op, val, 0, LVAL_TYPE_NUMBER);
    LASSERT_TYPE(op, val, 1, LVAL_TYPE_NUMBER);
    
    long res = -1;
    if (strcmp(op, "<") == 0) res = val->cell[0]->number < val->cell[1]->number ? 1 : 0;
    if (strcmp(op, "<=") == 0) res = val->cell[0]->number <= val->cell[1]->number ? 1 : 0;
    if (strcmp(op, ">") == 0) res = val->cell[0]->number > val->cell[1]->number ? 1 : 0;
    if (strcmp(op, ">=") == 0) res = val->cell[0]->number >= val->cell[1]->number ? 1 : 0;

    lval_free(val);
    return lval_make_number(res);
}

struct lval*
builtin_lt(struct lenv* env, struct lval* val)
{
    return builtin_ord(env, val, "<");
}

struct lval*
builtin_lte(struct lenv* env, struct lval* val)
{
    return builtin_ord(env, val, "<=");
}

struct lval*
builtin_gt(struct lenv* env, struct lval* val)
{
    return builtin_ord(env, val, ">");
}

struct lval*
builtin_gte(struct lenv* env, struct lval* val)
{
    return builtin_ord(env, val, ">=");
}

struct lval*
builtin_cmp(struct lenv* env, struct lval* val, const char* op)
{
    LASSERT_ARITY(op, val, 2);

    long res = -1;
    if (strcmp(op, "==") == 0) res = lval_eq(val->cell[0], val->cell[1]);
    if (strcmp(op, "!=") == 0) res = !lval_eq(val->cell[0], val->cell[1]);

    lval_free(val);
    return lval_make_number(res);
}

struct lval*
builtin_eq(struct lenv* env, struct lval* val)
{
    return builtin_cmp(env, val, "==");
}

struct lval*
builtin_neq(struct lenv* env, struct lval* val)
{
    return builtin_cmp(env, val, "!=");
}

struct lval*
builtin_if(struct lenv* env, struct lval* val)
{
    LASSERT_ARITY("if", val, 3);
    LASSERT_TYPE("if", val, 0, LVAL_TYPE_NUMBER);
    LASSERT_TYPE("if", val, 1, LVAL_TYPE_QEXPR);
    LASSERT_TYPE("if", val, 2, LVAL_TYPE_QEXPR);

    // mark both expressions as evaluable
    val->cell[1]->type = LVAL_TYPE_SEXPR;
    val->cell[2]->type = LVAL_TYPE_SEXPR;

    struct lval* res = NULL;
    if (val->cell[0]->number) {
        // if condition is true then eval the first expression
        res = eval(env, lval_list_pop(val, 1));
    } else {
        // else eval the second expression
        res = eval(env, lval_list_pop(val, 2));
    }

    lval_free(val);
    return res;
}

struct lval*
builtin_load(struct lenv* env, struct lval* val)
{
    LASSERT_ARITY("load", val, 1);
    LASSERT_TYPE("load", val, 0, LVAL_TYPE_STRING);

    FILE* f = fopen(val->cell[0]->string, "rb");
    if (f == NULL) {
        struct lval* error = lval_make_error("load failed: %s", val->cell[0]->string);
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
    struct lval* expr = lval_read_expr(input, &pos, '\0');
    free(input);

    if (expr->type != LVAL_TYPE_ERROR) {
        while (expr->cell_count > 0) {
            struct lval* x = eval(env, lval_list_pop(expr, 0));
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

struct lval*
builtin_display(struct lenv* env, struct lval* val)
{
    for (long i = 0; i < val->cell_count; i++) {
        lval_print(val->cell[i]);
        putchar(' ');
    }

    lval_free(val);

    return lval_make_sexpr();
}

struct lval*
builtin_newline(struct lenv* env, struct lval* val)
{
    putchar('\n');
    lval_free(val);

    return lval_make_sexpr();
}

struct lval*
builtin_error(struct lenv* env, struct lval* val)
{
    LASSERT_ARITY("error", val, 1);
    LASSERT_TYPE("error", val, 0, LVAL_TYPE_STRING);

    struct lval* error = lval_make_error(val->cell[0]->string);

    lval_free(val);
    return error;
}

struct lval*
builtin_list(struct lenv* env, struct lval* val)
{
    val->type = LVAL_TYPE_QEXPR;
    return val;
}

struct lval*
builtin_head(struct lenv* env, struct lval* val)
{
    LASSERT_ARITY("head", val, 1);
    LASSERT_TYPE("head", val, 0, LVAL_TYPE_QEXPR);
    LASSERT_NOT_EMPTY("head", val, 0);

    struct lval* head = lval_list_take(val, 0);
    while (head->cell_count > 1) lval_free(lval_list_pop(head, 1));
    return head;
}

struct lval*
builtin_tail(struct lenv* env, struct lval* val)
{
    LASSERT_ARITY("tail", val, 1);
    LASSERT_TYPE("tail", val, 0, LVAL_TYPE_QEXPR);
    LASSERT_NOT_EMPTY("tail", val, 0);

    struct lval* tail = lval_list_take(val, 0);
    lval_free(lval_list_pop(tail, 0));
    return tail;
}

struct lval*
builtin_join(struct lenv* env, struct lval* val)
{
    for (long i = 0; i < val->cell_count; i++ ) {
        LASSERT_TYPE("join", val, i, LVAL_TYPE_QEXPR);
    }

    struct lval* list = lval_list_pop(val, 0);
    while (val->cell_count) {
        list = lval_list_join(list, lval_list_pop(val, 0));
    }
    lval_free(val);

    return list;
}

struct lval*
builtin_eval(struct lenv* env, struct lval* val)
{
    LASSERT_ARITY("eval", val, 1);
    LASSERT_TYPE("eval", val, 0, LVAL_TYPE_QEXPR);

    struct lval* sexpr = lval_list_take(val, 0);
    sexpr->type = LVAL_TYPE_SEXPR;
    return eval(env, sexpr);
}

struct lval*
builtin_var(struct lenv* env, struct lval* val, const char* func)
{
    LASSERT_TYPE(func, val, 0, LVAL_TYPE_QEXPR);

    struct lval* symbols = val->cell[0];
    for (long i = 0; i < symbols->cell_count; i++) {
        LASSERTF(val, symbols->cell[i]->type == LVAL_TYPE_SYMBOL,
            "function '%s' cannot define non-symbol: "
            "got %s at index %i",
            func, lval_type_name(symbols->cell[i]->type), i);
    }

    LASSERTF(val, symbols->cell_count == val->cell_count - 1,
        "function '%s' passed mismatched number of symbols and values: "
        "want %i, got %i",
        func, val->cell_count - 1, symbols->cell_count);

    // assign copies of valects to symbols
    for (long i = 0; i < symbols->cell_count; i++) {
        if (strcmp(func, "def") == 0) {
            lenv_def(env, symbols->cell[i], val->cell[i + 1]);
        }
        if (strcmp(func, "=") == 0) {
            lenv_put(env, symbols->cell[i], val->cell[i + 1]);
        }
    }

    lval_free(val);
    return lval_make_sexpr();
}

struct lval*
builtin_def(struct lenv* env, struct lval* val)
{
    return builtin_var(env, val, "def");
}

struct lval*
builtin_put(struct lenv* env, struct lval* val)
{
    return builtin_var(env, val, "=");
}

struct lval*
builtin_lambda(struct lenv* env, struct lval* val)
{
    LASSERT_ARITY("lambda", val, 2);
    LASSERT_TYPE("lambda", val, 0, LVAL_TYPE_QEXPR);
    LASSERT_TYPE("lambda", val, 1, LVAL_TYPE_QEXPR);

    // ensure first qexpr contains only symbols
    for (long i = 0; i < val->cell[0]->cell_count; i++) {
        LASSERT(val, val->cell[0]->cell[i]->type == LVAL_TYPE_SYMBOL, "function 'lambda' cannot define non-symbol");
    }

    struct lval* formals = lval_list_pop(val, 0);
    struct lval* body = lval_list_pop(val, 0);
    lval_free(val);

    return lval_make_lambda(formals, body);
}

struct lval*
builtin_make_window(struct lenv* env, struct lval* val)
{
    LASSERT_ARITY("make-window", val, 3);
    LASSERT_TYPE("make-window", val, 0, LVAL_TYPE_STRING);
    LASSERT_TYPE("make-window", val, 1, LVAL_TYPE_NUMBER);
    LASSERT_TYPE("make-window", val, 2, LVAL_TYPE_NUMBER);

    const char* title = val->cell[0]->string;
    long width = val->cell[1]->number;
    long height = val->cell[2]->number;

    struct lval* window = lval_make_window(title, width, height);
    lval_free(val);
    return window;
}

struct lval*
call(struct lenv* env, struct lval* func, struct lval* val)
{
    // if builtin then just call it
    if (func->builtin != NULL) {
        return func->builtin(env, val);
    }

    long given = val->cell_count;
    long total = func->formals->cell_count;

    while (val->cell_count > 0) {
        LASSERTF(val, func->formals->cell_count > 0,
            "function passed too many args: want %i, got %i",
            total, given);

        // match next symbol with next value and add to env
        struct lval* symbol = lval_list_pop(func->formals, 0);

        // special case to deal with '&'
        if (strcmp(symbol->symbol, "&") == 0) {
            // ensure '&' is followed by a symbol
            if (func->formals->cell_count != 1) {
                lval_free(val);
                return lval_make_error("invalid function format: symbol '&' not followed by single symbol");
            }

            // bind next formal to a list of the remaining args
            struct lval* group = lval_list_pop(func->formals, 0);
            lenv_put(func->env, group, builtin_list(env, val));
            lval_free(symbol);
            lval_free(group);
            break;
        }

        struct lval* value = lval_list_pop(val, 0);
        lenv_put(func->env, symbol, value);
        lval_free(symbol);
        lval_free(value);
    }

    // args are all bound so orig list can be freed
    lval_free(val);

    // if '&' remains in formals then bind it to an empty list
    if (func->formals->cell_count > 0 && strcmp(func->formals->cell[0]->symbol, "&") == 0) {
        // ensure '&' is followed by a symbol
        if (func->formals->cell_count != 2) {
            return lval_make_error("invalid function format: symbol '&' not followed by single symbol");
        }

        // pop and delete '&' symbol
        lval_free(lval_list_pop(func->formals, 0));

        // pop next symbol and create empty list
        struct lval* symbol = lval_list_pop(func->formals, 0);
        struct lval* value = lval_make_qexpr();

        // bind to env and delete
        lenv_put(func->env, symbol, value);
        lval_free(symbol);
        lval_free(value);
    }

    // eval if all args have been bound
    if (func->formals->cell_count == 0) {
        // set the parent env and eval the body
        func->env->parent = env;
        return builtin_eval(func->env, lval_list_append(lval_make_sexpr(), lval_copy(func->body)));
    } else {
        // otherwise return the partially evaluated function
        return lval_copy(func);
    }
}

struct lval*
eval_sexpr(struct lenv* env, struct lval* val)
{
    // eval children
    for (long i = 0; i < val->cell_count; i++) {
        val->cell[i] = eval(env, val->cell[i]);
    }

    // error checking
    for (long i = 0; i < val->cell_count; i++) {
        if (val->cell[i]->type == LVAL_TYPE_ERROR) return lval_list_take(val, i);
    }

    // empty expression
    if (val->cell_count == 0) return val;

    // single expression
//    if (val->cell_count == 1) return lval_list_take(val, 0);

    // ensure first element is a symbol
    struct lval* func = lval_list_pop(val, 0);
    if (func->type != LVAL_TYPE_FUNC) {
        lval_free(func);
        lval_free(val);
        return lval_make_error(
            "s-expressions starts with invalid type: want %s, got %s",
            lval_type_name(LVAL_TYPE_FUNC), lval_type_name(func->type)); 
    }

    // call builtin with operator
    struct lval* result = call(env, func, val);
    lval_free(func);
    return result;
}

struct lval*
eval(struct lenv* env, struct lval* val)
{
    if (val->type == LVAL_TYPE_SYMBOL) {
        struct lval* v = lenv_get(env, val);
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
    struct lval* k = lval_make_symbol(name);
    struct lval* v = lval_make_builtin(func);
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
    struct lval* res = builtin_load(env,
        lval_list_append(lval_make_sexpr(), lval_make_string("prelude.sqky")));
    lval_free(res);

    if (argc >= 2) {
        for (long i = 1; i < argc; i++) {
            struct lval* args = lval_list_append(lval_make_sexpr(), lval_make_string(argv[i]));
            struct lval* x = builtin_load(env, args);
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
            struct lval* expr = lval_read_expr(line, &pos, '\0');
            while (expr->cell_count > 0) {
                struct lval* res = eval(env, lval_list_pop(expr, 0));
                if (res->type == LVAL_TYPE_SEXPR && res->cell_count == 0) {
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
