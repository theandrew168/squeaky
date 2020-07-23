#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lval.h"
#include "lval_number.h"

bool
lval_number_init(struct lval_number* val, long number)
{
    assert(val != NULL);

    val->type = LVAL_TYPE_NUMBER;
    val->number = number;
    return true;
}

void
lval_number_free(struct lval_number* val)
{
    assert(val != NULL);

    // nothing to free because longs are embedded in lval_number
}

void
lval_number_copy(const struct lval_number* val, struct lval_number* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->number = val->number;
}

void
lval_number_print(const struct lval_number* val)
{
    assert(val != NULL);

    printf("%ld", val->number);
}

bool
lval_number_equal(const struct lval_number* a, const struct lval_number* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return a->number == b->number;
}
