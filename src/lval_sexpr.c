#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lval.h"
#include "lval_sexpr.h"

bool
lval_sexpr_init(union lval* val)
{
    assert(val != NULL);

    struct lval_sexpr* v = AS_SEXPR(val);

    v->type = LVAL_TYPE_SEXPR;
    v->count = 0;
    v->list = NULL;

    return true;
}

void
lval_sexpr_free(union lval* val)
{
    assert(val != NULL);

    struct lval_sexpr* v = AS_SEXPR(val);

    // free all children
    for (long i = 0; i < v->count; i++) {
        lval_free(v->list[i]);
    }

    // free the list itself
    free(v->list);
}

void
lval_sexpr_copy(const union lval* val, union lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    const struct lval_sexpr* v = AS_CONST_SEXPR(val);
    struct lval_sexpr* c = AS_SEXPR(copy);

    c->type = v->type;
    c->count = v->count;

    // copy each child
    for (long i = 0; i < v->count; i++) {
        c->list[i] = lval_copy(v->list[i]);
    }
}

void
lval_sexpr_print(const union lval* val)
{
    assert(val != NULL);

    const struct lval_sexpr* v = AS_CONST_SEXPR(val);

    putchar('(');
    for (long i = 0; i < v->count; i++) {
        lval_print(v->list[i]); 
        if (i != (v->count - 1)) putchar(' ');
    }
    putchar(')');
}

bool
lval_sexpr_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    const struct lval_sexpr* aa = AS_CONST_SEXPR(a);
    const struct lval_sexpr* bb = AS_CONST_SEXPR(a);

    // not equal if sizes aren't equal
    if (aa->count != bb->count) return false;

    // check each pair of children for equality
    for (long i = 0; i < aa->count; i++) {
        if (!lval_equal(aa->list[i], bb->list[i])) return false;
    }

    return false;
}
