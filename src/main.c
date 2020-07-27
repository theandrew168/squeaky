#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "chunk.h"
#include "vm.h"

int
main(int argc, char* argv[])
{
    struct vm vm = { 0 };
    vm_init(&vm);

    struct chunk chunk = { 0 };
    chunk_init(&chunk);

    long constant = chunk_add_constant(&chunk, 1.2);
    chunk_write(&chunk, OP_CONSTANT, 123);
    chunk_write(&chunk, constant, 123);

    constant = chunk_add_constant(&chunk, 3.4);
    chunk_write(&chunk, OP_CONSTANT, 123);
    chunk_write(&chunk, constant, 123);

    chunk_write(&chunk, OP_ADD, 123);

    constant = chunk_add_constant(&chunk, 5.6);
    chunk_write(&chunk, OP_CONSTANT, 123);
    chunk_write(&chunk, constant, 123);

    chunk_write(&chunk, OP_DIVIDE, 123);
    chunk_write(&chunk, OP_NEGATE, 123);

    chunk_write(&chunk, OP_RETURN, 123);

    chunk_disassemble(&chunk, "test chunk");
    vm_interpret(&vm, &chunk);

    chunk_free(&chunk);
    vm_free(&vm);

    return EXIT_SUCCESS;
}
