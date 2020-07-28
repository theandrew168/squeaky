#ifndef SQUEAKY_VM_H_INCLUDED
#define SQUEAKY_VM_H_INCLUDED

#include "chunk.h"
#include "value.h"

enum {
    STACK_SIZE = 256,
};

struct vm {
    struct value* stack[STACK_SIZE];
    long stack_top;
};

enum vm_status {
    VM_OK = 0,
    VM_ERROR,
};

void vm_init(struct vm* vm);
void vm_free(struct vm* vm);

int vm_execute(struct vm* vm, struct chunk* chunk);

#endif
