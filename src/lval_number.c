#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lval.h"
#include "lval_number.h"

bool
lval_number_init(union lval* val, long number)
{
    assert(val != NULL);

    struct lval_number* v = AS_NUMBER(val);

    v->type = LVAL_TYPE_NUMBER;
    v->number = number;

    return true;
}

void
lval_number_free(union lval* val)
{
    assert(val != NULL);

    // nothing to free because longs are embedded in lval_number
}

void
lval_number_copy(const union lval* val, union lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    const struct lval_number* v = AS_CONST_NUMBER(val);
    struct lval_number* c = AS_NUMBER(copy);

    c->type = v->type;
    c->number = v->number;
}

void
lval_number_print(const union lval* val)
{
    assert(val != NULL);

    const struct lval_number* v = AS_CONST_NUMBER(val);

    printf("%ld", v->number);
}

bool
lval_number_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    const struct lval_number* aa = AS_CONST_NUMBER(a);
    const struct lval_number* bb = AS_CONST_NUMBER(b);

    return aa->number == bb->number;
}
