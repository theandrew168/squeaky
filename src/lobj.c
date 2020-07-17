#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lobj.h"

struct lobj*
lobj_make_error(const char* error)
{
    struct lobj* obj = malloc(sizeof(struct lobj));
    obj->type = LOBJ_TYPE_ERROR;
    obj->error = malloc(strlen(error) + 1);
    strcpy(obj->error, error);
    return obj;
}

struct lobj*
lobj_make_number(long number)
{
    struct lobj* obj = malloc(sizeof(struct lobj));
    obj->type = LOBJ_TYPE_NUMBER;
    obj->number = number;
    return obj;
}

struct lobj*
lobj_make_symbol(const char* symbol)
{
    struct lobj* obj = malloc(sizeof(struct lobj));
    obj->type = LOBJ_TYPE_SYMBOL;
    obj->symbol = malloc(strlen(symbol) + 1);
    strcpy(obj->symbol, symbol);
    return obj;
}

struct lobj*
lobj_make_sexpr(void)
{
    struct lobj* obj = malloc(sizeof(struct lobj));
    obj->type = LOBJ_TYPE_SEXPR;
    obj->cell_count = 0;
    obj->cell = NULL;
    return obj;
}

void
lobj_free(struct lobj* obj)
{
    assert(obj != NULL);

    if (obj->type == LOBJ_TYPE_ERROR) free(obj->error);
    if (obj->type == LOBJ_TYPE_SYMBOL) free(obj->symbol);
    if (obj->type == LOBJ_TYPE_SEXPR) {
        for (long i = 0; i < obj->cell_count; i++) {
            lobj_free(obj->cell[i]);
        }
        free(obj->cell);
    }

    free(obj);
}

void
lobj_print(const struct lobj* obj)
{
    assert(obj != NULL);

    switch (obj->type) {
        case LOBJ_TYPE_ERROR: printf("error: %s", obj->error); break;
        case LOBJ_TYPE_NUMBER: printf("%ld", obj->number); break;
        case LOBJ_TYPE_SYMBOL: printf("%s", obj->symbol); break;
        case LOBJ_TYPE_SEXPR:
            putchar('(');
            for (long i = 0; i < obj->cell_count; i++) {
                lobj_print(obj->cell[i]);
                if (i != (obj->cell_count - 1)) putchar(' ');
            }
            putchar(')');
            break;
        default: printf("undefined lisp object"); break;
    }
}

void
lobj_println(const struct lobj* obj)
{
    assert(obj != NULL);

    lobj_print(obj);
    putchar('\n');
}

void
lobj_sexpr_append(struct lobj* sexpr, struct lobj* obj)
{
    assert(sexpr != NULL);
    assert(obj != NULL);

    sexpr->cell_count++;
    sexpr->cell = realloc(sexpr->cell, sizeof(struct obj*) * sexpr->cell_count);
    sexpr->cell[sexpr->cell_count - 1] = obj;
}

struct lobj*
lobj_sexpr_pop(struct lobj* sexpr, long i)
{
    assert(sexpr != NULL);
    assert(i >= 0);
    assert(i < sexpr->cell_count);

    struct lobj* obj = sexpr->cell[i];
    memmove(&sexpr->cell[i], &sexpr->cell[i + 1], sizeof(struct lobj*) * (sexpr->cell_count - i - 1));
    sexpr->cell_count--;
    sexpr->cell = realloc(sexpr->cell, sizeof(struct lobj*) * sexpr->cell_count);
    return obj;
}

struct lobj*
lobj_sexpr_take(struct lobj* sexpr, long i)
{
    struct lobj* obj = lobj_sexpr_pop(sexpr, i);
    lobj_free(sexpr);
    return obj;
}
