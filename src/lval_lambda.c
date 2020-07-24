#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lenv.h"
#include "lval.h"
#include "lval_lambda.h"

bool
lval_lambda_init(struct lval* val, struct lval* formals, struct lval* body)
{
    assert(val != NULL);
    assert(formals != NULL);
    assert(body != NULL);

    val->type = LVAL_TYPE_LAMBDA;
    val->as.lambda.env = lenv_make();
    val->as.lambda.formals = formals;
    val->as.lambda.body = body;

    return true;
}

void
lval_lambda_free(struct lval* val)
{
    assert(val != NULL);

    lenv_free(val->as.lambda.env);
    lval_free(val->as.lambda.formals);
    lval_free(val->as.lambda.body);
}

void
lval_lambda_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    copy->as.lambda.env = lenv_copy(val->as.lambda.env);
    copy->as.lambda.formals = lval_copy(val->as.lambda.formals);
    copy->as.lambda.body = lval_copy(val->as.lambda.body);
}

void
lval_lambda_print(const struct lval* val)
{
    assert(val != NULL);

    printf("(lambda ");
    lval_print(val->as.lambda.formals);
    putchar(' ');
    lval_print(val->as.lambda.body);
    putchar(')');
}

bool
lval_lambda_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return lval_equal(a->as.lambda.formals, b->as.lambda.formals) &&
           lval_equal(a->as.lambda.body, b->as.lambda.body);
}
