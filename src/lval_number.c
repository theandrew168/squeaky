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

    val->type = LVAL_TYPE_NUMBER;
    AS_NUMBER(val)->number = number;

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

    copy->type = val->type;
    copy->number = AS_NUMBER(val)->number;
}

void
lval_number_print(const union lval* val)
{
    assert(val != NULL);

    printf("%ld", AS_NUMBER(val)->number);
}

bool
lval_number_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return AS_NUMBER(a)->number == AS_NUMBER(b)->number;
}
