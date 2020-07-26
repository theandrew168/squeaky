#ifndef SQUEAKY_CHUNK_H_INCLUDED
#define SQUEAKY_CHUNK_H_INCLUDED

#include <stdint.h>

#include "value.h"

enum opcode {
    OP_UNDEFINED = 0,
    OP_CONSTANT,
    OP_RETURN,
};

struct chunk {
    long count;
    long capacity;
    uint8_t* code;
    long* lines;
    struct value_array constants;
};

void chunk_init(struct chunk* chunk);
void chunk_free(struct chunk* chunk);

void chunk_write(struct chunk* chunk, uint8_t byte, long line);
long chunk_add_constant(struct chunk* chunk, Value value);

void chunk_disassemble(const struct chunk* chunk, const char* name);

#endif
