#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lval.h"
#include "lval_list.h"

bool
lval_list_init(struct lval* val, int type)
{
    assert(val != NULL);

    v->type = type;
    v->count = 0;
    v->list = NULL;

    return true;
}

void
lval_list_free(struct lval* val)
{
    assert(val != NULL);

    // free all children
    for (long i = 0; i < val->count; i++) {
        lval_free(val->list[i]);
    }

    // free the list itself
    free(val->list);
}

void
lval_list_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    copy->count = val->count;

    // copy each child
    for (long i = 0; i < val->count; i++) {
        copy->list[i] = lval_copy(val->list[i]);
    }
}

void
lval_list_print(const struct lval* val, char start, char end)
{
    assert(val != NULL);

    putchar(start);
    for (long i = 0; i < val->count; i++) {
        lval_print(val->list[i]); 
        if (i != (val->count - 1)) {
            putchar(' ');
        }
    }
    putchar(end);
}

bool
lval_list_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    // not equal if sizes aren't equal
    if (a->count != b->count) return false;

    // check each pair of children for equality
    for (long i = 0; i < a->count; i++) {
        if (!lval_equal(a->list[i], b->list[i])) return false;
    }

    return false;
}

struct lval*
lval_list_append(struct lval* list, struct lval* val)
{
    assert(list != NULL);
    assert(val != NULL);

    list->count++;
    list->list = realloc(list->list, sizeof(struct lval*) * list->count);
    list->list[list->count - 1] = val;
    return list;
}

struct lval*
lval_list_pop(struct lval* list, long i)
{
    assert(list != NULL);
    assert(i >= 0);
    assert(i < list->count);

    struct lval* val = list->list[i];
    memmove(&list->list[i], &list->list[i + 1], sizeof(struct lval*) * (list->count - i - 1));

    list->count--;
    list->list = realloc(list->list, sizeof(struct lval*) * list->count);
    return val;
}

struct lval*
lval_list_take(struct lval* list, long i)
{
    assert(list != NULL);

    struct lval* val = lval_list_pop(list, i);
    lval_free(list);
    return val;
}

struct lval*
lval_list_join(struct lval* list, struct lval* extras)
{
    assert(list != NULL);
    assert(extras != NULL);

    while (extras->count > 0) {
        list = lval_list_append(list, lval_list_pop(extras, 0));
    }

    lval_free(extras);
    return list;
}
