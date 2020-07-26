#ifndef SQUEAKY_CHUNK_H_INCLUDED
#define SQUEAKY_CHUNK_H_INCLUDED

#include <stdint.h>

#include "value.h"

// bobscheme opcodes:
//  1. OP_CONST    - lookup constant and push it onto the stack
//  2. OP_LOADVAR  - lookup var's name, lookup value in the env, and push it onto the stack
//  3. OP_STOREVAR - pop value from stack, lookup var's name, put value into the env
//  4. OP_DEFVAR   - pop value from stack, lookup var's name, def value info the env (top-level)
//  5. OP_POP      - pop value from stack (thats it)
//  6. OP_JUMP     - set pc (or ip) to the instruction's arg
//  7. OP_FJUMP    - pop value (bool) from the stack, jump based on value's bool (true or false?)
//  8. OP_FUNCTION - lookup function (as constant), create value (closure), push value on stack
//  9. OP_RETURN   - pop frame from framestack
// 10. OP_CALL     - pop value from stack (builtin/closure), pop func args from the stack into
//                   some other local stack, if builtin func then exec and push resulting value,
//                   else (closure) extend closure's env with additional arg names / values, create
//                   a new frame and push it onto the frame stack (will get exec'd on next VM loop)

enum opcode {
    OP_UNDEFINED = 0,
    OP_CONSTANT,
    OP_NEGATE,
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
long chunk_disassemble_inst(const struct chunk* chunk, long offset);

#endif
