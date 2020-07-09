#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"

lval*
lval_num(long x)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval*
lval_sym(char* s)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval*
lval_sexpr(void)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval*
lval_err(char* m)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);
    return v;
}

void
lval_del(lval* v)
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

lval*
lval_read_num(mpc_ast_t* t)
{
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval*
lval_read(mpc_ast_t* t)
{
    // if symbol or number return conv to that type
    if (strstr(t->tag, "number")) return lval_read_num(t);
    if (strstr(t->tag, "symbol")) return lval_sym(t->contents);

    // if root (>) or sexpr then create empty list
    lval* x = NULL;
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

lval*
lval_add(lval* v, lval* x)
{
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

void
lval_expr_print(lval* v, char open, char close)
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
lval_print(lval* v)
{
    switch (v->type) {
        case LVAL_NUM:   printf("%li", v->num); break;
        case LVAL_SYM:   printf("%s", v->sym); break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
        case LVAL_ERR:   printf("Error: %s", v->err); break;
    }
}

void
lval_println(lval* v)
{
    lval_print(v);
    putchar('\n');
}
