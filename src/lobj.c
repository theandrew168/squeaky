#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lobj.h"

struct lobj*
lobj_make_error(const char* fmt, ...)
{
    struct lobj* obj = malloc(sizeof(struct lobj));
    obj->type = LOBJ_TYPE_ERROR;

    va_list va;
    va_start(va, fmt);

    obj->error = malloc(512);
    vsnprintf(obj->error, 511, fmt, va);
    obj->error = realloc(obj->error, strlen(obj->error) + 1);

    va_end(va);
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
lobj_make_func(lbuiltin func)
{
    struct lobj* obj = malloc(sizeof(struct lobj));
    obj->type = LOBJ_TYPE_FUNC;
    obj->func = func;
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

struct lobj*
lobj_make_qexpr(void)
{
    struct lobj* obj = malloc(sizeof(struct lobj));
    obj->type = LOBJ_TYPE_QEXPR;
    obj->cell_count = 0;
    obj->cell = NULL;
    return obj;
}

struct lobj*
lobj_copy(const struct lobj* obj)
{
    assert(obj != NULL);

    struct lobj* copy = malloc(sizeof(struct lobj));
    copy->type = obj->type;

    switch (obj->type) {
        case LOBJ_TYPE_ERROR:
            copy->error = malloc(strlen(obj->error) + 1);
            strcpy(copy->error, obj->error);
            break;
        case LOBJ_TYPE_NUMBER:
            copy->number = obj->number;
            break;
        case LOBJ_TYPE_SYMBOL:
            copy->symbol = malloc(strlen(obj->symbol) + 1);
            strcpy(copy->symbol, obj->symbol);
            break;
        case LOBJ_TYPE_FUNC:
            copy->func = obj->func;
            break;
        case LOBJ_TYPE_SEXPR:
        case LOBJ_TYPE_QEXPR:
            copy->cell_count = obj->cell_count;
            copy->cell = malloc(sizeof(struct lobj*) * obj->cell_count);
            for (long i = 0; i < obj->cell_count; i++) {
                copy->cell[i] = lobj_copy(obj->cell[i]);
            }
            break;
    }

    return copy;
}

void
lobj_free(struct lobj* obj)
{
    assert(obj != NULL);

    if (obj->type == LOBJ_TYPE_ERROR) free(obj->error);
    if (obj->type == LOBJ_TYPE_SYMBOL) free(obj->symbol);
    if (obj->type == LOBJ_TYPE_SEXPR || obj->type == LOBJ_TYPE_QEXPR) {
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
        case LOBJ_TYPE_FUNC: printf("<func>"); break;
        case LOBJ_TYPE_SEXPR:
            putchar('(');
            for (long i = 0; i < obj->cell_count; i++) {
                lobj_print(obj->cell[i]);
                if (i != (obj->cell_count - 1)) putchar(' ');
            }
            putchar(')');
            break;
        case LOBJ_TYPE_QEXPR:
            putchar('{');
            for (long i = 0; i < obj->cell_count; i++) {
                lobj_print(obj->cell[i]);
                if (i != (obj->cell_count - 1)) putchar(' ');
            }
            putchar('}');
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

struct lobj*
lobj_list_append(struct lobj* list, struct lobj* obj)
{
    assert(list != NULL);
    assert(obj != NULL);

    list->cell_count++;
    list->cell = realloc(list->cell, sizeof(struct obj*) * list->cell_count);
    list->cell[list->cell_count - 1] = obj;
    return list;
}

struct lobj*
lobj_list_pop(struct lobj* list, long i)
{
    assert(list != NULL);
    assert(i >= 0);
    assert(i < list->cell_count);

    struct lobj* obj = list->cell[i];
    memmove(&list->cell[i], &list->cell[i + 1], sizeof(struct lobj*) * (list->cell_count - i - 1));
    list->cell_count--;
    list->cell = realloc(list->cell, sizeof(struct lobj*) * list->cell_count);
    return obj;
}

struct lobj*
lobj_list_take(struct lobj* list, long i)
{
    struct lobj* obj = lobj_list_pop(list, i);
    lobj_free(list);
    return obj;
}

struct lobj*
lobj_list_join(struct lobj* list, struct lobj* extras)
{
    while (extras->cell_count) {
        list = lobj_list_append(list, lobj_list_pop(extras, 0));
    }

    lobj_free(extras);
    return list;
}
