#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "value.h"
#include "vm.h"

void
vm_init(struct vm* vm)
{
    assert(vm != NULL);

    vm->stack_top = 0;
}

void
vm_free(struct vm* vm)
{
    assert(vm != NULL);
}

static void
vm_stack_push(struct vm* vm, struct value* value)
{
    assert(vm != NULL);
    assert(vm->stack_top <= STACK_SIZE);

    vm->stack[vm->stack_top] = value;
    vm->stack_top++;
}

static struct value*
vm_stack_pop(struct vm* vm)
{
    assert(vm != NULL);
    assert(vm->stack_top > 0);

    vm->stack_top--;
    return vm->stack[vm->stack_top];
}

static int
vm_run(struct vm* vm, struct chunk* chunk)
{
    // initialize instruction pointer to start of code
    uint8_t* ip = chunk->code;

    for (;;) {
        uint8_t inst = *ip++;
        switch (inst) {
            case OP_CONSTANT: {
                struct value* value = chunk->constants.values[*ip++];
                vm_stack_push(vm, value);
                break;
            }
            case OP_NEG: {
                struct value* value = vm_stack_pop(vm);
                value->as.number = -value->as.number;
                vm_stack_push(vm, value);
            }
            case OP_RETURN: {
                struct value* value = vm_stack_pop(vm);
                value_println(value);
                return VM_OK;
            }
        }
    }

    return VM_OK;
}
