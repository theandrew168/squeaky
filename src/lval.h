#ifndef LISPY_LVAL_H_INCLUDED
#define LISPY_LVAL_H_INCLUDED

enum {
    LVAL_UNDEFINED = 0,
    LVAL_NUM,
    LVAL_ERR,
};

enum {
    LERR_UNDEFINED = 0,
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM,
};

typedef struct {
    int type;
    long num;
    int err;
} lval;

lval lval_num(long num);
lval lval_err(int err);
void lval_print(lval v);
void lval_println(lval v);

#endif
