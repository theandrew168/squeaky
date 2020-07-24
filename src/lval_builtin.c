#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_builtin.h"

bool
lval_builtin_init(union lval* val, lbuiltin builtin)
{
    assert(val != NULL);
    assert(builtin != NULL);

    struct lval_builtin* v = AS_BUILTIN(val);

    v->type = LVAL_TYPE_BUILTIN;
    v->builtin = builtin;

    return true;
}

void
lval_builtin_free(union lval* val)
{
    assert(val != NULL);

    // nothing to free because a builtin is just a func ptr
}

void
lval_builtin_copy(const union lval* val, union lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    const struct lval_builtin* v = AS_CONST_BUILTIN(val);
    struct lval_builtin* c = AS_BUILTIN(val);

    c->type = v->type;
    c->builtin = v->builtin;
}

void
lval_builtin_print(const union lval* val)
{
    assert(val != NULL);

    printf("<builtin>");
}

bool
lval_builtin_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    const struct lval_builtin* aa = AS_CONST_BUILTIN(a);
    const struct lval_builtin* bb = AS_CONST_BUILTIN(b);

    return aa->builtin == bb->builtin;
}
