#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lval.h"
#include "lval_qexpr.h"

bool
lval_qexpr_init(struct lval_qexpr* val)
{
    assert(val != NULL);

    val->type = LVAL_TYPE_QEXPR;
    val->count = 0;
    val->list = NULL;
    return true;
}

void
lval_qexpr_free(struct lval_qexpr* val)
{
    assert(val != NULL);

    // free all children
    for (long i = 0; i < val->count; i++) {
        lval_free(val->list[i]);
    }

    // free the list itself
    free(val->list);
}

void
lval_qexpr_copy(const struct lval_qexpr* val, struct lval_qexpr* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    // copy each child
    copy->count = val->count;
    for (long i = 0; i < val->count; i++) {
        copy->list[i] = lval_copy(val->list[i]);
    }
}

void
lval_qexpr_print(const struct lval_qexpr* val)
{
    assert(val != NULL);

    putchar('{');
    for (long i = 0; i < val->count; i++) {
        lval_print(val->list[i]); 
        if (i != (val->count - 1)) putchar(' ');
    }
    putchar('}');
}

bool
lval_qexpr_equal(const struct lval_qexpr* a, const struct lval_qexpr* b)
{
    assert(a != NULL);
    assert(b != NULL);

    // not equal if sizes aren't equal
    if (a->count != b->count) return false;

    // check each pair of children for equality
    for (long i = 0; i < a->count; i++) {
        if (!lval_eq(a->list[i], b->list[i])) return false;
    }

    return false;
}
