#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "lexer.h"
#include "value.h"
#include "vm.h"

void
vm_init(struct vm* vm)
{
    assert(vm != NULL);

    vm->stack_top = vm->stack;
}

void
vm_free(struct vm* vm)
{
    assert(vm != NULL);
}

void
vm_stack_push(struct vm* vm, Value value)
{
    assert(vm != NULL);

    *vm->stack_top = value;
    vm->stack_top++;
}

Value
vm_stack_pop(struct vm* vm)
{
    assert(vm != NULL);

    vm->stack_top--;
    return *vm->stack_top;
}

static int
vm_run(struct vm* vm)
{
// define a few helper macros specifically for this function
// the tightness of this loop discourages unnecessary function calls
#define READ_BYTE(vm) (*(vm)->ip++)
#define READ_CONSTANT(vm) ((vm)->chunk->constants.values[READ_BYTE(vm)])
#define BINARY_OP(vm, op)         \
  do {                            \
    double b = vm_stack_pop(vm);  \
    double a = vm_stack_pop(vm);  \
    vm_stack_push(vm, (a op b));  \
  } while (0)

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm->stack; slot < vm->stack_top; slot++) {
            printf("[ ");
            value_print(*slot);
            printf(" ]");
        }
        printf("\n");
        chunk_disassemble_inst(vm->chunk, (long)(vm->ip - vm->chunk->code));
#endif

        uint8_t inst;
        switch (inst = READ_BYTE(vm)) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT(vm);
                vm_stack_push(vm, constant);
                break;
            }
            case OP_ADD:      BINARY_OP(vm, +); break;
            case OP_SUBTRACT: BINARY_OP(vm, -); break;
            case OP_MULTIPLY: BINARY_OP(vm, *); break;
            case OP_DIVIDE:   BINARY_OP(vm, /); break;
            case OP_NEGATE: {
                Value value = vm_stack_pop(vm);
                vm_stack_push(vm, -value);
                break;
            }
            case OP_RETURN: {
                value_println(vm_stack_pop(vm));
                return VM_OK;
            }
        }
    }

    return VM_OK;

#undef BINARY_OP
#undef READ_BYTE
#undef READ_CONSTANT
}

int
vm_interpret(struct vm* vm, const char* source)
{
    assert(vm != NULL);
    assert(source != NULL);

    struct lexer lexer = { 0 };
    lexer_init(&lexer, source);

    long line = -1;
    for (;;) {
        struct token token = lexer_next_token(&lexer);
        if (token.line != line) {
            printf("%4ld ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }

        printf("%-10s '%.*s'\n", lexer_token_name(token.type), (int)token.length, token.start);
        if (token.type == TOKEN_EOF) break;
    }

    return VM_OK;
}
