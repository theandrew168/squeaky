#ifndef SQUEAKY_LVAL_LAMBDA_H_INCLUDED
#define SQUEAKY_LVAL_LAMBDA_H_INCLUDED

#include <stdbool.h>

struct lenv;
struct lval;

struct lval_lambda {
    int type;
    struct lenv* env;
    struct lval* formals;
    struct lval* body;
};

bool lval_lambda_init(struct lval_lambda* val, struct lval* formals, struct lval* body);
void lval_lambda_free(struct lval_lambda* val);
void lval_lambda_copy(const struct lval_lambda* val, struct lval_lambda* copy);

void lval_lambda_print(const struct lval_lambda* val);
bool lval_lambda_equal(const struct lval_lambda* a, const struct lval_lambda* b);

#endif
