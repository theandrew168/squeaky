#include <stdio.h>

#include "io.h"
#include "reader.h"
#include "value.h"

int
main(void)
{
    for (;;) {
        printf("> ");
        struct value* exp = reader_read(stdin);
        if (value_is_eof(exp)) break;
        io_writeln(exp);
    }
}
