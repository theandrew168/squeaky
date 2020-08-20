#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "env.h"
#include "mce.h"
#include "reader.h"
#include "value.h"
#include "vm.h"

int
main()
{
    struct vm vm = { 0 };
    vm_init(&vm);

    struct value* env = env_empty(&vm);
    env_define(&vm, vm_make_symbol(&vm, "foo"), vm_make_string(&vm, "foo"), env);
    env_define(&vm, vm_make_symbol(&vm, "bar"), vm_make_number(&vm, 42), env);

    struct value* v = vm_make_number(&vm, 123);
    value_println(stdout, v);

    struct value* exp = reader_read(&vm, stdin);
    value_println(stdout, exp);

    struct value* res = mce_eval(&vm, exp, env);
    value_println(stdout, res);

    vm_free(&vm);
    return EXIT_SUCCESS;
}
