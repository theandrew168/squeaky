#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "value.h"

void
chunk_init(struct chunk* chunk)
{
    assert(chunk != NULL);

    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    value_array_init(&chunk->constants);
}

void
chunk_free(struct chunk* chunk)
{
    assert(chunk != NULL);

    free(chunk->code);
    value_array_free(&chunk->constants);
    chunk_init(chunk);
}

void
chunk_write(struct chunk* chunk, uint16_t op, uint16_t arg)
{
    assert(chunk != NULL);

    // if new count will exceed the current capacity, grow the buffer
    if (chunk->count + 1 > chunk->capacity) {
        if (chunk->capacity == 0) {
            // special case for first growth from zero to 8 (just a reasonable default)
            chunk->capacity = 8;
        } else {
            // else double the buffer's capacity
            chunk->capacity *= 2;
        }

        // realloc the buffer to the new capacity
        chunk->code = realloc(chunk->code, sizeof(uint32_t) * chunk->capacity);
        assert(chunk->code != NULL);
    }

    // now that the buffer is big enough, append the new instruction
    chunk->code[chunk->count] = ((uint32_t)op << 16) | arg;
    chunk->count++;
}

long
chunk_add_constant(struct chunk* chunk, struct value* value)
{
    assert(chunk != NULL);

    value_array_append(&chunk->constants, value);
    return chunk->constants.count - 1;
}

//static long
//disassemble_simple_inst(const char* name, long offset)
//{
//    printf("%s\n", name);
//    return offset + 1;
//}
//
//static long
//disassemble_constant_inst(const char* name, const struct chunk* chunk, long offset)
//{
//    uint8_t constant = chunk->code[offset + 1];
//    printf("%-16s %4d '", name, constant);
//    value_print(chunk->constants.values[constant]);
//    printf("'\n");
//    return offset + 2;
//}
//
//void
//chunk_disassemble(const struct chunk* chunk, const char* name)
//{
//    printf("== %s ==\n", name);
//
//    for (long offset = 0; offset < chunk->count;) {
//        offset = chunk_disassemble_inst(chunk, offset);
//    }
//}
//
//long
//chunk_disassemble_inst(const struct chunk* chunk, long offset)
//{
//    // print the current offset
//    printf("%04ld ", offset);
//
//    // print the current source line or pipe if unchanged
//    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
//        printf("   | ");
//    } else {
//        printf("%4ld ", chunk->lines[offset]);
//    }
//
//    uint8_t instruction = chunk->code[offset];
//    switch (instruction) {
//        case OP_CONSTANT:
//            return disassemble_constant_inst("OP_CONSTANT", chunk, offset);
//        case OP_RETURN:
//            return disassemble_simple_inst("OP_RETURN", offset);
//        default:
//            printf("Unknown opcode %d\n", instruction);
//            return offset + 1;
//    }
//}
