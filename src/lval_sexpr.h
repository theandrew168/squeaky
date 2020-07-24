#ifndef SQUEAKY_LVAL_SEXPR_H_INCLUDED
#define SQUEAKY_LVAL_SEXPR_H_INCLUDED

#include <stdbool.h>

union lval;

struct lval_sexpr {
    int type;
    long count;
    union lval** list;
};

#define AS_SEXPR(val) ((struct lval_sexpr*)(val))
#define AS_CONST_SEXPR(val) ((const struct lval_sexpr*)(val))

bool lval_sexpr_init(union lval* val);
void lval_sexpr_free(union lval* val);
void lval_sexpr_copy(const union lval* val, union lval* copy);

void lval_sexpr_print(const union lval* val);
bool lval_sexpr_equal(const union lval* a, const union lval* b);

#endif
