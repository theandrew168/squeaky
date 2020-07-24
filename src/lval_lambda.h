#ifndef SQUEAKY_LVAL_LAMBDA_H_INCLUDED
#define SQUEAKY_LVAL_LAMBDA_H_INCLUDED

#include <stdbool.h>

struct lenv;
union lval;

struct lval_lambda {
    int type;
    struct lenv* env;
    union lval* formals;
    union lval* body;
};

#define AS_LAMBDA(val) ((struct lval_lambda*)(val))
#define AS_CONST_LAMBDA(val) ((const struct lval_lambda*)(val))

bool lval_lambda_init(union lval* val, union lval* formals, union lval* body);
void lval_lambda_free(union lval* val);
void lval_lambda_copy(const union lval* val, union lval* copy);

void lval_lambda_print(const union lval* val);
bool lval_lambda_equal(const union lval* a, const union lval* b);

#endif
