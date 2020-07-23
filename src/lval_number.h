#ifndef SQUEAKY_LVAL_NUMBER_H_INCLUDED
#define SQUEAKY_LVAL_NUMBER_H_INCLUDED

#include <stdbool.h>

// TODO: this can be expanded with GMP for big number funsies
struct lval_number {
    int type;
    long number;
};

bool lval_number_init(struct lval_number* val, long number);
void lval_number_free(struct lval_number* val);
void lval_number_copy(const struct lval_number* val, struct lval_number* copy);

void lval_number_print(const struct lval_number* val);
bool lval_number_equal(const struct lval_number* a, const struct lval_number* b);

#endif
