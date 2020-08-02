#include <assert.h>
#include <stdlib.h>

#include "builtin.h"
#include "value.h"

struct value*
builtin_plus(struct value* args)
{
    long sum = 0;
    for (; args != NULL; args = cdr(args)) {
        sum += car(args)->as.number;
    }

    return value_make_number(sum);
}

struct value*
builtin_multiply(struct value* args)
{
    long product = 1;
    for (; args != NULL; args = cdr(args)) {
        product *= car(args)->as.number;
    }

    return value_make_number(product);
}
