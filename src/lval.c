#include <stdio.h>

#include "lval.h"

lval
lval_num(long num)
{
    lval v = {
        .type = LVAL_NUM,
        .num = num,
    };
    return v;
}

lval
lval_err(int err)
{
    lval v = {
        .type = LVAL_ERR,
        .err = err,
    };
    return v;
}

void
lval_print(lval v)
{
    switch (v.type) {
    case LVAL_NUM:
        printf("%li", v.num);
        break;
    case LVAL_ERR:
        if (v.err == LERR_DIV_ZERO) printf("Error: Division By Zero!");
        if (v.err == LERR_BAD_OP) printf("Error: Invalid Operator!");
        if (v.err == LERR_BAD_NUM) printf("Error: Invalid Number!");
        break;
    }
}

void
lval_println(lval v)
{
    lval_print(v);
    putchar('\n');
}
