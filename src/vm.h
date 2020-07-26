#ifndef SQUEAKY_VM_H_INCLUDED
#define SQUEAKY_VM_H_INCLUDED

#include "chunk.h"
#include "value.h"

enum {
    STACK_SIZE = 256,
};

struct vm {
    const struct chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_SIZE];
    Value* stack_top;
};

enum vm_status {
    VM_OK = 0,
    VM_ERROR_COMPILE,
    VM_ERROR_RUNTIME,
};

void vm_init(struct vm* vm);
void vm_free(struct vm* vm);

void vm_stack_push(struct vm* vm, Value value);
Value vm_stack_pop(struct vm* vm);

int vm_interpret(struct vm* vm, const struct chunk* chunk);

#endif
