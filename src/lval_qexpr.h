#ifndef SQUEAKY_LVAL_QEXPR_H_INCLUDED
#define SQUEAKY_LVAL_QEXPR_H_INCLUDED

#include <stdbool.h>

union lval;

struct lval_qexpr {
    int type;
    long count;
    union lval** list;
};

#define AS_QEXPR(val) ((struct lval_qexpr*)(val))
#define AS_CONST_QEXPR(val) ((const struct lval_qexpr*)(val))

bool lval_qexpr_init(union lval* val);
void lval_qexpr_free(union lval* val);
void lval_qexpr_copy(const union lval* val, union lval* copy);

void lval_qexpr_print(const union lval* val);
bool lval_qexpr_equal(const union lval* a, const union lval* b);

#endif
