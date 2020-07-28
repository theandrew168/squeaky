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
    return VM_OK;
}

//int
//vm_interpret(struct vm* vm, const char* source)
//{
//    assert(vm != NULL);
//    assert(source != NULL);
//
//    struct lexer lexer = { 0 };
//    lexer_init(&lexer, source);
//
//    struct parser parser = { 0 };
//    parser_init(&parser, &lexer);
//
//    parser_print(&parser);
//
////    long line = -1;
////    for (;;) {
////        struct token token = lexer_next_token(&lexer);
////        if (token.line != line) {
////            printf("%4ld ", token.line);
////            line = token.line;
////        } else {
////            printf("   | ");
////        }
////
////        printf("%-10s '%.*s'\n", lexer_token_name(token.type), (int)token.length, token.start);
////        if (token.type == TOKEN_EOF) break;
////    }
//
//    return VM_OK;
//}
