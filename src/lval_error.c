#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_error.h"

bool
lval_error_init(struct lval_error* val, const char* fmt, ...)
{
    assert(val != NULL);
    assert(fmt != NULL);

    va_list va;
    va_start(va, fmt);

    // alloc enough memory to hold a reasonable error message, copy the
    // error message in, and then shrink it down to size
    val->type = LVAL_TYPE_ERROR;
    val->error = malloc(1024);
    vsnprintf(val->error, 1023, fmt, va);
    val->error = realloc(val->error, strlen(val->error) + 1);

    va_end(va);
    return true;
}

void
lval_error_free(struct lval_error* val)
{
    assert(val != NULL);

    free(val->error);
}

void
lval_error_copy(const struct lval_error* val, struct lval_error* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->error = malloc(strlen(val->error) + 1);
    strcpy(copy->error, val->error);
}

void
lval_error_print(const struct lval_error* val)
{
    assert(val != NULL);

    printf("error: %s", val->error);
}

bool
lval_error_equal(const struct lval_error* a, const struct lval_error* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return strcmp(a->error, b->error) == 0;
}
