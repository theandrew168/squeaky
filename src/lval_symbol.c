#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_symbol.h"

bool
lval_symbol_init(union lval* val, const char* symbol)
{
    assert(val != NULL);
    assert(symbol != NULL);

    struct lval_symbol* v = AS_SYMBOL(val);

    v->type = LVAL_TYPE_SYMBOL;
    v->symbol = malloc(strlen(symbol) + 1);
    strcpy(v->symbol, symbol);
    return true;
}

void
lval_symbol_free(union lval* val)
{
    assert(val != NULL);

    struct lval_symbol* v = AS_SYMBOL(val);

    free(v->symbol);
}

void
lval_symbol_copy(const union lval* val, union lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    const struct lval_symbol* v = AS_CONST_SYMBOL(val);
    struct lval_symbol* c = AS_SYMBOL(copy);

    c->type = v->type;
    c->symbol = malloc(strlen(v->symbol) + 1);
    strcpy(c->symbol, v->symbol);
}

void
lval_symbol_print(const union lval* val)
{
    assert(val != NULL);

    const struct lval_symbol* v = AS_CONST_SYMBOL(val);

    printf("%s", v->symbol);
}

bool
lval_symbol_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    const struct lval_symbol* aa = AS_CONST_SYMBOL(a);
    const struct lval_symbol* bb = AS_CONST_SYMBOL(b);

    return strcmp(aa->symbol, bb->symbol) == 0;
}
