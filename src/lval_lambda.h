#ifndef SQUEAKY_LVAL_LAMBDA_H_INCLUDED
#define SQUEAKY_LVAL_LAMBDA_H_INCLUDED

#include <stdbool.h>

struct lval;

bool lval_lambda_init(struct lval* val, struct lval* formals, struct lval* body);
void lval_lambda_free(struct lval* val);
void lval_lambda_copy(const struct lval* val, struct lval* copy);

void lval_lambda_print(const struct lval* val);
bool lval_lambda_equal(const struct lval* a, const struct lval* b);

#endif
