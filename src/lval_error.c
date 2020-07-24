#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_error.h"

bool
lval_error_init(union lval* val, const char* fmt, va_list args)
{
    assert(val != NULL);
    assert(fmt != NULL);

    struct lval_error* v = AS_ERROR(val);

    // alloc enough memory to hold a reasonable error message, copy the
    // error message in, and then shrink it down to size
    v->type = LVAL_TYPE_ERROR;
    v->error = malloc(1024);
    vsnprintf(v->error, 1023, fmt, args);
    v->error = realloc(v->error, strlen(v->error) + 1);

    return true;
}

void
lval_error_free(union lval* val)
{
    assert(val != NULL);

    struct lval_error* v = AS_ERROR(val);

    free(v->error);
}

void
lval_error_copy(const union lval* val, union lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    const struct lval_error* v = AS_CONST_ERROR(val);
    struct lval_error* c = AS_ERROR(copy);

    c->type = v->type;
    c->error = malloc(strlen(v->error) + 1);
    strcpy(c->error, v->error);
}

void
lval_error_print(const union lval* val)
{
    assert(val != NULL);

    const struct lval_error* v = AS_CONST_ERROR(val);

    printf("error: %s", v->error);
}

bool
lval_error_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    const struct lval_error* aa = AS_CONST_ERROR(a);
    const struct lval_error* bb = AS_CONST_ERROR(b);

    return strcmp(aa->error, bb->error) == 0;
}
