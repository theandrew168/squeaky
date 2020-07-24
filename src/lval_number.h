#ifndef SQUEAKY_LVAL_NUMBER_H_INCLUDED
#define SQUEAKY_LVAL_NUMBER_H_INCLUDED

#include <stdbool.h>

struct lval;

bool lval_number_init(struct lval* val, long number);
void lval_number_free(struct lval* val);
void lval_number_copy(const struct lval* val, struct lval* copy);

void lval_number_print(const struct lval* val);
bool lval_number_equal(const struct lval* a, const struct lval* b);

#endif
