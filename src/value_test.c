#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "value.h"

bool
test_value_make_number(void)
{
    struct value* value = value_make_number(42);
    assert(value != NULL);

    if (!value_is_number(value)) {
        fprintf(stderr, "value_is_number check failed\n");
        return false;
    }

    return true;
}

bool
test_list_make(void)
{
    struct value* a = value_make_symbol("a");
    struct value* b = value_make_symbol("b");
    struct value* c = value_make_symbol("c");

    struct value* list = list_make(a, b, c, NULL);

    if (!value_equal(car(list), a)) {
        fprintf(stderr, "list_make first element is wrong\n");
        return false;
    }

    if (!value_equal(cadr(list), b)) {
        fprintf(stderr, "list_make second element is wrong\n");
        return false;
    }

    if (!value_equal(caddr(list), c)) {
        fprintf(stderr, "list_make third element is wrong\n");
        return false;
    }

    return true;
}
