#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lval.h"
#include "lval_number.h"

bool
lval_number_init(struct lval* val, long number)
{
    assert(val != NULL);

    val->type = LVAL_TYPE_NUMBER;
    val->as.number = number;

    return true;
}

void
lval_number_free(struct lval* val)
{
    assert(val != NULL);

    // nothing to free because longs are embedded in lval_number
}

void
lval_number_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    copy->as.number = val->as.number;
}

void
lval_number_print(const struct lval* val)
{
    assert(val != NULL);

    printf("%ld", val->as.number);
}

bool
lval_number_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return a->as.number == b->as.number;
}
