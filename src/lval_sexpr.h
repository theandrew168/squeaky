#ifndef SQUEAKY_LVAL_SEXPR_H_INCLUDED
#define SQUEAKY_LVAL_SEXPR_H_INCLUDED

#include <stdbool.h>

struct lval;

struct lval_sexpr {
    int type;
    long count;
    struct lval** list;
};

bool lval_sexpr_init(struct lval_sexpr* val);
void lval_sexpr_free(struct lval_sexpr* val);
void lval_sexpr_copy(const struct lval_sexpr* val, struct lval_sexpr* copy);

void lval_sexpr_print(const struct lval_sexpr* val);
bool lval_sexpr_equal(const struct lval_sexpr* a, const struct lval_sexpr* b);

#endif
