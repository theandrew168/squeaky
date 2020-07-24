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
    AS_SYMBOL(val)->symbol = malloc(strlen(symbol) + 1);
    strcpy(AS_SYMBOL(val)->symbol, symbol);
    return true;
}

void
lval_symbol_free(struct lval* val)
{
    assert(val != NULL);

    free(AS_SYMBOL(val)->symbol);
}

void
lval_symbol_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    AS_SYMBOL(copy)->symbol = malloc(strlen(AS_SYMBOL(val)->symbol) + 1);
    strcpy(AS_SYMBOL(copy)->symbol, AS_SYMBOL(val)->symbol);
}

void
lval_symbol_print(const struct lval* val)
{
    assert(val != NULL);

    printf("%s", AS_SYMBOL(val)->symbol);
}

bool
lval_symbol_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return strcmp(AS_SYMBOL(a)->symbol, AS_SYMBOL(b)->symbol) == 0;
}
