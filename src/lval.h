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

struct lval {
    int type;
    long num;

    char* err;
    char* sym;

    long count;
    struct lval** cell;
};

struct lval* lval_num(long x);
struct lval* lval_sym(char* s);
struct lval* lval_sexpr(void);
struct lval* lval_err(char* m);

void lval_del(struct lval* v);

struct lval* lval_read_num(mpc_ast_t* t);
struct lval* lval_read(mpc_ast_t* t);
struct lval* lval_add(struct lval* v, struct lval* x);

struct lval* lval_eval_sexpr(struct lval* v);
struct lval* lval_eval(struct lval* v);
struct lval* builtin_op(struct lval* a, char* op);

struct lval* lval_pop(struct lval* v, long i);
struct lval* lval_take(struct lval* v, long i);

void lval_expr_print(struct lval* v, char open, char close);
void lval_print(struct lval* v);
void lval_println(struct lval* v);

#endif
