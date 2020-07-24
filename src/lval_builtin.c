#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_builtin.h"

bool
lval_builtin_init(struct lval* val, lbuiltin builtin)
{
    assert(val != NULL);
    assert(builtin != NULL);

    val->type = LVAL_TYPE_BUILTIN;
    val->as.builtin = builtin;

    return true;
}

void
lval_builtin_free(struct lval* val)
{
    assert(val != NULL);

    // nothing to free because a builtin is just a func ptr
}

void
lval_builtin_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    copy->as.builtin = val->as.builtin;
}

void
lval_builtin_print(const struct lval* val)
{
    assert(val != NULL);

    printf("<builtin>");
}

bool
lval_builtin_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return a->as.builtin == b->as.builtin;
}
