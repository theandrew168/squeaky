#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"

struct lval*
lval_num(long x)
{
    struct lval* v = malloc(sizeof(struct lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

struct lval*
lval_sym(char* s)
{
    struct lval* v = malloc(sizeof(struct lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

struct lval*
lval_sexpr(void)
{
    struct lval* v = malloc(sizeof(struct lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

struct lval*
lval_err(char* m)
{
    struct lval* v = malloc(sizeof(struct lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);
    return v;
}

void
lval_del(struct lval* v)
{
    switch (v->type) {
        case LVAL_NUM: break;
        case LVAL_SYM: free(v->sym); break;
        case LVAL_SEXPR:
            for (long i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
            break;
        case LVAL_ERR: free(v->err); break;
    }

    free(v);
}

struct lval*
lval_read_num(mpc_ast_t* t)
{
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

struct lval*
lval_read(mpc_ast_t* t)
{
    // if symbol or number return conv to that type
    if (strstr(t->tag, "number")) return lval_read_num(t);
    if (strstr(t->tag, "symbol")) return lval_sym(t->contents);

    // if root (>) or sexpr then create empty list
    struct lval* x = NULL;
    if (strcmp(t->tag, ">") == 0) x = lval_sexpr();
    if (strstr(t->tag, "sexpr")) x = lval_sexpr();

    // fill the list with any valid expression contained within
    for (long i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) continue;
        if (strcmp(t->children[i]->contents, ")") == 0) continue;
        if (strcmp(t->children[i]->tag, "regex") == 0) continue;
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

struct lval*
lval_add(struct lval* v, struct lval* x)
{
    v->count++;
    v->cell = realloc(v->cell, sizeof(struct lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

struct lval*
lval_eval_sexpr(struct lval* v)
{
    // eval children
    for (long i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    // error checking
    for (long i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) return lval_take(v, i);
    }

    // empty expression
    if (v->count == 0) return v;

    // single expression
    if (v->count == 1) return lval_take(v, 0);

    // ensure first element is a symbol
    struct lval* f = lval_pop(v, 0);
    if (f->type != LVAL_SYM) {
        lval_del(f);
        lval_del(v);
        return lval_err("s-expression doesn't start with symbol");
    }

    // call building with operator
    struct lval* result = builtin_op(v, f->sym);
    lval_del(f);
    return result;
}

struct lval*
lval_eval(struct lval* v)
{
    if (v->type == LVAL_SEXPR) return lval_eval_sexpr(v);
    return v;
}

struct lval*
builtin_op(struct lval* a, char* op)
{
    // ensure all args are numbers
    for (long i = 0; i < a->count; i++) {
        if (a->cell[i]->type != LVAL_NUM) {
            lval_del(a);
            return lval_err("cannot operate on non-number");
        }
    }

    // pop the first ele
    struct lval* x = lval_pop(a, 0);

    // if no args and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    while (a->count > 0) {
        struct lval* y = lval_pop(a, 0);

        if (strcmp(op, "+") == 0) x->num += y->num;
        if (strcmp(op, "-") == 0) x->num -= y->num;
        if (strcmp(op, "*") == 0) x->num *= y->num;
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval_del(x);
                lval_del(y);
                x = lval_err("division by zero");
                break;
            }
            x->num /= y->num;
        }

        lval_del(y);
    }

    lval_del(a);
    return x;
}

struct lval*
lval_pop(struct lval* v, long i)
{
    struct lval* x = v->cell[i];
    memmove(&v->cell[i], &v->cell[i + 1], sizeof(struct lval*) * (v->count - i - 1));
    v->count--;
    v->cell = realloc(v->cell, sizeof(struct lval*) * v->count);
    return x;
}

struct lval*
lval_take(struct lval* v, long i)
{
    struct lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

void
lval_expr_print(struct lval* v, char open, char close)
{
    putchar(open);
    for (long i = 0; i < v->count; i++) {
        // print value within cell
        lval_print(v->cell[i]);
        // don't print tailing space on last element
        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

void
lval_print(struct lval* v)
{
    switch (v->type) {
        case LVAL_NUM:   printf("%li", v->num); break;
        case LVAL_SYM:   printf("%s", v->sym); break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
        case LVAL_ERR:   printf("Error: %s", v->err); break;
    }
}

void
lval_println(struct lval* v)
{
    lval_print(v);
    putchar('\n');
}
