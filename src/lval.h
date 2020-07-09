#ifndef LISPY_LVAL_H_INCLUDED
#define LISPY_LVAL_H_INCLUDED

#include "mpc.h"

enum {
    LVAL_UNDEFINED = 0,
    LVAL_NUM,
    LVAL_SYM,
    LVAL_SEXPR,
    LVAL_ERR,
};

enum {
    LERR_UNDEFINED = 0,
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM,
};

typedef struct lval {
    int type;
    long num;

    char* err;
    char* sym;

    long count;
    struct lval** cell;
} lval;

lval* lval_num(long x);
lval* lval_sym(char* s);
lval* lval_sexpr(void);
lval* lval_err(char* m);

void lval_del(lval* v);

lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
lval* lval_add(lval* v, lval* x);

void lval_expr_print(lval* v, char open, char close);
void lval_print(lval* v);
void lval_println(lval* v);

#endif
