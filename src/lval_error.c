#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_error.h"

bool
lval_error_init(struct lval* val, const char* fmt, va_list args)
{
    assert(val != NULL);
    assert(fmt != NULL);

    // alloc enough memory to hold a reasonable error message, copy the
    // error message in, and then shrink it down to size
    val->type = LVAL_TYPE_ERROR;
    val->as.error = malloc(1024);
    vsnprintf(val->as.error, 1023, fmt, args);
    val->as.error = realloc(val->as.error, strlen(val->as.error) + 1);

    return true;
}

void
lval_error_free(struct lval* val)
{
    assert(val != NULL);

    free(val->as.error);
}

void
lval_error_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    copy->as.error = malloc(strlen(val->as.error) + 1);
    strcpy(copy->as.error, val->as.error);
}

void
lval_error_print(const struct lval* val)
{
    assert(val != NULL);

    printf("error: %s", val->as.error);
}

bool
lval_error_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return strcmp(a->as.error, b->as.error) == 0;
}
