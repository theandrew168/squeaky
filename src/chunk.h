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

// lexical structure (R5RS 7.1.1):
// -------------------------------
//
// <token> : <identifier>
//         | <boolean>
//         | <number>
//         | <character>
//         | <string>
//         | /#(/
//         | /,@/
//         | /[()'`,.]/
//
// <comment> : /;[^\n]*/
//
// <identifier> : <initial> <subsequent>*
//              | <peculiar_identifier>
// <initial>    : <letter>
//              | <special_initial>
// <letter>     : /[A-Za-z]/
//
// <special_initial>     : /[!$%&*.:<=>?^_~]/
// <subsequent>          : <initial>
//                       | <digit>
//                       | <special_subsequent>
// <digit>               : /[0-9]/
// <special_subsequent>  : /[+-.@]/
// <peculiar_identifier> : /([+-]|\.\.\.)/
//
// <syntactic_keyword>  : <expression_keyword>
//                      | /else/
//                      | /=>/
//                      | /define/
//                      | /unquote/
//                      | /unquote-splicing/
// <expression_keyword> : /quote/
//                      | /lambda/
//                      | /if/
//                      | /set!/
//                      | /begin/
//                      | /cond/
//                      | /and/
//                      | /or/
//                      | /case/
//                      | /let/
//                      | /let*/
//                      | /letrec/
//                      | /do/
//                      | /delay/
//                      | /quasiquote/
//
// <variable> : <identifier> NOT <syntactic_keyword>
//
// <boolean> : '#t'
//           | '#f'
//
// <character>      : /#\<any_character>/
//                  | /#\<character_name>/
// <any_character>  : /[!-~]/
// <character_name> : 'space'
//                  | 'newline'
//
// <string>         : '"' <string_element>* '"'
// <string_element> : /([^"\]|\"|\\)/
//
// <number> : <radix2>  <digit2>
//          | <radix8>  <digit8>
//          | <radix10> <digit10>
//          | <radix16> <digit16>
//
// <radix2>  : /#b/
// <radix8>  : /#o/
// <radix10> : /(#d)?/
// <radix16> : /#x/
//
// <digit2>  : /[0-1]/
// <digit8>  : /[0-7]/
// <digit10> : /[0-9]/
// <digit16> : /[0-9A-Fa-f]/

// external representation (R5RS 7.1.2):
// -------------------------------------
//
// <datum>          : <simple_datum>
//                  | <compound_datum>
// <simple_datum>   : <boolean>
//                  | <number>
//                  | <character>
//                  | <string>
//                  | <symbol>
// <symbol> : <identifier>
// <compound_datum> : <list>
//                  | <vector>
// <list>           : '(' <datum>* ')'
//                  | '(' <datum>+ '.' <datum> ')'
//                  | <abbreviation>
// <abbreviation>   : <abbrev_prefix> <datum>
// <abbrev_prefix>  : /(['`,]|,@)/
// <vector>         : '#(' <datum>* ')'

enum opcode {
    OP_UNDEFINED = 0,
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
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
