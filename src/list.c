#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "value.h"

struct value*
list_make(long count, struct value* value, ...)
{
    va_list args;
    va_start(args, value);

    struct value* head = cons(value, value_make_empty_list());
    struct value* tail = head;

    // i starts at 1 because the first element is part of the initial 'head'
    for (long i = 1; i < count; i++) { 
        struct value* v = va_arg(args, struct value*);
        tail->as.pair.cdr = cons(v, tail->as.pair.cdr);
        tail = cdr(tail);
    }

    va_end(args);
    return head;
}

long
list_length(const struct value* list)
{
    long count = 0;

    const struct value* iter = list;
    while (!value_is_empty_list(iter)) {
        count++;
        iter = cdr(iter);
    }

    return count;
}

struct value*
list_nth(const struct value* list, long n)
{
    if (n < 0 || n >= list_length(list)) {
        fprintf(stderr, "list: invalid index: %ld\n", n);
        exit(EXIT_FAILURE);
    }

    const struct value* iter = list;
    for (long i = 0; i < n; i++) {
        iter = cdr(iter);
    }

    return car(iter);
}

struct value*
list_car(const struct value* list)
{
    if (value_is_empty_list(list)) {
        fprintf(stderr, "the primitive 'car' is defined only for non-empty lists\n");
        exit(EXIT_FAILURE);
    }

    return list->as.pair.car;
}

struct value*
list_cdr(const struct value* list)
{
    if (value_is_empty_list(list)) {
        fprintf(stderr, "the primitive 'cdr' is defined only for non-empty lists\n");
        exit(EXIT_FAILURE);
    }

    return list->as.pair.cdr;
}
