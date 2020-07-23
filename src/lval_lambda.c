#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lenv.h"
#include "lval.h"
#include "lval_lambda.h"

bool
lval_lambda_init(struct lval_lambda* val, struct lval* formals, struct lval* body)
{
    assert(val != NULL);
    assert(formals != NULL);
    assert(body != NULL);

    val->type = LVAL_TYPE_LAMBDA;
    val->env = lenv_make();
    val->formals = formals;
    val->body = body;
    return true;
}

void
lval_lambda_free(struct lval_lambda* val)
{
    assert(val != NULL);

    lenv_free(val->env);
    lval_free(val->formals);
    lval_free(val->body);
}

void
lval_lambda_copy(const struct lval_lambda* val, struct lval_lambda* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->env = lenv_copy(val->env);
    copy->formals = lval_copy(val->formals);
    copy->body = lval_copy(val->body);
}

void
lval_lambda_print(const struct lval_lambda* val)
{
    assert(val != NULL);

    printf("(lambda ");
    lval_print(val->formals);
    putchar(' ');
    lval_print(val->body);
    putchar(')');
}

bool
lval_lambda_equal(const struct lval_lambda* a, const struct lval_lambda* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return lval_eq(a->formals, b->formals) && lval_eq(a->body, b->body);
}
