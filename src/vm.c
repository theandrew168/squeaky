#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "value.h"
#include "vm.h"

enum {
    GC_UNUSED = 0,
    GC_USED,
    GC_MARK,

    GC_WHITE,
    GC_GRAY,
    GC_BLACK,
};

void
vm_init(struct vm* vm)
{
    assert(vm != NULL);

    vm->count = 0;
    vm->capacity = 1024 * 1024;
    vm->heap = calloc(vm->capacity, sizeof(Value));
}

void
vm_free(struct vm* vm)
{
    assert(vm != NULL);

    free(vm->heap);
    vm->count = 0;
    vm->capacity = 0;
    vm->heap = NULL;
}
