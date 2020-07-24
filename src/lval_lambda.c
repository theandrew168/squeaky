#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lenv.h"
#include "lval.h"
#include "lval_lambda.h"

bool
lval_lambda_init(union lval* val, union lval* formals, union lval* body)
{
    assert(val != NULL);
    assert(formals != NULL);
    assert(body != NULL);

    struct lval_lambda* v = AS_LAMBDA(val);

    v->type = LVAL_TYPE_LAMBDA;
    v->env = lenv_make();
    v->formals = formals;
    v->body = body;

    return true;
}

void
lval_lambda_free(union lval* val)
{
    assert(val != NULL);

    struct lval_lambda* v = AS_LAMBDA(val);

    lenv_free(v->env);
    lval_free(v->formals);
    lval_free(v->body);
}

void
lval_lambda_copy(const union lval* val, union lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    const struct lval_lambda* v = AS_CONST_LAMBDA(val);
    struct lval_lambda* c = AS_LAMBDA(copy);

    c->type = v->type;
    c->env = lenv_copy(v->env);
    c->formals = lval_copy(v->formals);
    c->body = lval_copy(v->body);
}

void
lval_lambda_print(const union lval* val)
{
    assert(val != NULL);

    const struct lval_lambda* v = AS_LAMBDA(val);

    printf("(lambda ");
    lval_print(v->formals);
    putchar(' ');
    lval_print(v->body);
    putchar(')');
}

bool
lval_lambda_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    const struct lval_lambda* aa = AS_CONST_LAMBDA(a);
    const struct lval_lambda* bb = AS_CONST_LAMBDA(b);

    return lval_equal(aa->formals, bb->formals) && lval_equal(aa->body, bb->body);
}
