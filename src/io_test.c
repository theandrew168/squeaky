#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "list.h"
#include "value.h"

bool
test_io_read(void)
{
    const struct {
        const char* str;
        struct value* exp;
    } tests[] = {
        { .str = "()",
          .exp = EMPTY_LIST },
        { .str = "'()",
          .exp = list_make(2, value_make_symbol("quote"), EMPTY_LIST) },
        { .str = "'foo",
          .exp = list_make(2, value_make_symbol("quote"), value_make_symbol("foo")) },
        { .str = "#t",
          .exp = value_make_boolean(true) },
        { .str = "#f",
          .exp = value_make_boolean(false) },
        { .str = "12345",
          .exp = value_make_number(12345) },
        { .str = "\"foo\"",
          .exp = value_make_string("foo") },
        { .str = "foo",
          .exp = value_make_symbol("foo") },
        { .str = "(1 2 3)",
          .exp = list_make(3, value_make_number(1), value_make_number(2), value_make_number(3)) },
        { .str = "  ( 1 2   3 )",
          .exp = list_make(3, value_make_number(1), value_make_number(2), value_make_number(3)) },
    };
    long num_tests = sizeof(tests) / sizeof(tests[0]);

    for (long i = 0; i < num_tests; i++) {
        long consumed = 0;
        struct value* exp = io_read(tests[i].str, &consumed);
        if (!value_equal(exp, tests[i].exp) || consumed != (long)strlen(tests[i].str)) {
            fprintf(stderr, "io_read: misread expr: %s\n", tests[i].str);
            return false;
        }
    }

    return true;
}
