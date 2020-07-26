#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "chunk.h"

int
main(int argc, char* argv[])
{
    struct chunk chunk = { 0 };
    chunk_init(&chunk);

    long constant = chunk_add_constant(&chunk, 1.2);
    chunk_write(&chunk, OP_CONSTANT, 123);
    chunk_write(&chunk, constant, 123);
    chunk_write(&chunk, OP_RETURN, 123);
    chunk_disassemble(&chunk, "test chunk");

    chunk_free(&chunk);

    return EXIT_SUCCESS;
}
