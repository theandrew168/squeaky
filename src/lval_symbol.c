#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_symbol.h"

bool
lval_symbol_init(struct lval* val, const char* symbol)
{
    assert(val != NULL);
    assert(symbol != NULL);

    val->type = LVAL_TYPE_SYMBOL;
    val->as.symbol = malloc(strlen(symbol) + 1);
    strcpy(val->as.symbol, symbol);
    return true;
}

void
lval_symbol_free(struct lval* val)
{
    assert(val != NULL);

    free(val->as.symbol);
}

void
lval_symbol_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    copy->as.symbol = malloc(strlen(val->as.symbol) + 1);
    strcpy(copy->as.symbol, val->as.symbol);
}

void
lval_symbol_print(const struct lval* val)
{
    assert(val != NULL);

    printf("%s", val->as.symbol);
}

bool
lval_symbol_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return strcmp(a->as.symbol, b->as.symbol) == 0;
}
