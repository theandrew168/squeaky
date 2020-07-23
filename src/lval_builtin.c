#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_builtin.h"

bool
lval_builtin_init(struct lval_builtin* val, lbuiltin builtin)
{
    assert(val != NULL);
    assert(builtin != NULL);

    val->type = LVAL_TYPE_BUILTIN;
    val->builtin = builtin;
    return true;
}

void
lval_builtin_free(struct lval_builtin* val)
{
    assert(val != NULL);

    // nothing to free because a builtin is just a func ptr
}

void
lval_builtin_copy(const struct lval_builtin* val, struct lval_builtin* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->builtin = val->builtin;
}

void
lval_builtin_print(const struct lval_builtin* val)
{
    assert(val != NULL);

    printf("<builtin>");
}

bool
lval_builtin_equal(const struct lval_builtin* a, const struct lval_builtin* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return a->builtin == b->builtin;
}
