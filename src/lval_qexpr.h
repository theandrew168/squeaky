#ifndef SQUEAKY_LVAL_QEXPR_H_INCLUDED
#define SQUEAKY_LVAL_QEXPR_H_INCLUDED

#include <stdbool.h>

struct lval;

struct lval_qexpr {
    int type;
    long count;
    struct lval** list;
};

bool lval_qexpr_init(struct lval_qexpr* val);
void lval_qexpr_free(struct lval_qexpr* val);
void lval_qexpr_copy(const struct lval_qexpr* val, struct lval_qexpr* copy);

void lval_qexpr_print(const struct lval_qexpr* val);
bool lval_qexpr_equal(const struct lval_qexpr* a, const struct lval_qexpr* b);

#endif
