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
