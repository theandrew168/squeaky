#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lval.h"
#include "lval_sexpr.h"

bool
lval_sexpr_init(struct lval_sexpr* val)
{
    assert(val != NULL);

    val->type = LVAL_TYPE_SEXPR;
    val->count = 0;
    val->list = NULL;
    return true;
}

void
lval_sexpr_free(struct lval_sexpr* val)
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
lval_sexpr_copy(const struct lval_sexpr* val, struct lval_sexpr* copy)
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
lval_sexpr_print(const struct lval_sexpr* val)
{
    assert(val != NULL);

    putchar('(');
    for (long i = 0; i < val->count; i++) {
        lval_print(val->list[i]); 
        if (i != (val->count - 1)) putchar(' ');
    }
    putchar(')');
}

bool
lval_sexpr_equal(const struct lval_sexpr* a, const struct lval_sexpr* b)
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
