#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_symbol.h"

bool
lval_symbol_init(struct lval_symbol* val, const char* symbol)
{
    assert(val != NULL);
    assert(symbol != NULL);

    val->type = LVAL_TYPE_SYMBOL;
    val->symbol = malloc(strlen(symbol) + 1);
    strcpy(val->symbol, symbol);
    return true;
}

void
lval_symbol_free(struct lval_symbol* val)
{
    assert(val != NULL);

    free(val->symbol);
}

void
lval_symbol_copy(const struct lval_symbol* val, struct lval_symbol* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->symbol = malloc(strlen(val->symbol) + 1);
    strcpy(copy->symbol, val->symbol);
}

void
lval_symbol_print(const struct lval_symbol* val)
{
    assert(val != NULL);

    printf("%s", val->symbol);
}

bool
lval_symbol_equal(const struct lval_symbol* a, const struct lval_symbol* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return strcmp(a->symbol, b->symbol) == 0;
}
