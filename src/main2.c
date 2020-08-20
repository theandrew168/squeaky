#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "value.h"
#include "vm.h"

int
main()
{
    struct vm vm = { 0 };
    vm_init(&vm);

    struct value* v = vm_make_number(&vm, 123);
    value_println(stdout, v);

    struct value* exp = reader_read(&vm, stdin);
    value_println(stdout, exp);

    vm_free(&vm);
    return EXIT_SUCCESS;
}
