#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "lexer.h"
#include "parser.h"

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

void
parser_init(struct parser* parser, struct lexer* lexer)
{
    assert(parser != NULL);
    assert(lexer != NULL);

    parser->chunk = NULL;
    parser->lexer = lexer;
    parser->had_error = false;
    parser->panic_mode = false;
}

static void
error_at(struct parser* parser, struct token* token, const char* message)
{
    if (parser->panic_mode) return;
    parser->panic_mode = true;

    fprintf(stderr, "[line %ld] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // no location
    } else {
        fprintf(stderr, " at '%.*s'", (int)token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser->had_error = true;
}

static void
error_at_current(struct parser* parser, const char* message)
{
    error_at(parser, &parser->current, message);
}

static void
error(struct parser* parser, const char* message)
{
    error_at(parser, &parser->previous, message);
}

// advance the parser to the next non-error token
static void
parser_advance(struct parser* parser)
{
    parser->previous = parser->current;
    for (;;) {
        parser->current = lexer_next_token(parser->lexer);
        if (parser->current.type != TOKEN_ERROR) break;

        error_at_current(parser, parser->current.start);
    }
}

static bool
parser_match(struct parser* parser, int type)
{
    if (parser->current.type != type) return false;
    parser_advance(parser);
    return true;
}

// advance the parser only if the current token type matches
static void
parser_consume(struct parser* parser, int type, const char* message)
{
    if (parser->current.type == type) {
        parser_advance(parser);
        return;
    }

    error_at_current(parser, message);
}

static bool print_datum(struct parser* parser);
static bool print_simple_datum(struct parser* parser);
static bool print_compound_datum(struct parser* parser);
static bool print_list(struct parser* parser);
static bool print_abbreviation(struct parser* parser);
static bool print_vector(struct parser* parser);

static bool
print_datum(struct parser* parser)
{
    switch (parser->current.type) {
        case TOKEN_BOOLEAN:
        case TOKEN_NUMBER:
        case TOKEN_CHARACTER:
        case TOKEN_STRING:
        case TOKEN_SYMBOL:
            return print_simple_datum(parser);
        case TOKEN_LPAREN: 
        case TOKEN_VECTOR:
            return print_compound_datum(parser);
        default: 
            error(parser, "invalid datum");
            return false;
    }
}

static bool
print_simple_datum(struct parser* parser)
{
    switch (parser->current.type) {
        case TOKEN_BOOLEAN:
            printf("%.*s", (int)parser->current.length, parser->current.start);
            break;
        case TOKEN_CHARACTER:
            printf("%.*s", (int)parser->current.length, parser->current.start);
            break;
        case TOKEN_NUMBER:
            printf("%.*s", (int)parser->current.length, parser->current.start);
            break;
        case TOKEN_SYMBOL:
            printf("%.*s", (int)parser->current.length, parser->current.start);
            break;
        case TOKEN_STRING:
            printf("%.*s", (int)parser->current.length, parser->current.start);
            break;
        default:
            error(parser, "invalid simple datum");
            return false;
    }

    parser_advance(parser);
    return true;
}

static bool
print_compound_datum(struct parser* parser)
{
    switch (parser->current.type) {
        case TOKEN_LPAREN: 
        case TOKEN_QUOTE:
            return print_list(parser);
        case TOKEN_VECTOR:
            return print_vector(parser);
        default:
            error(parser, "invalid compound datum");
            return false;
    }
}

static bool
print_list(struct parser* parser)
{
    // check for quote and parse abbrev if found
    if (parser->current.type == TOKEN_QUOTE) {
        return print_abbreviation(parser);
    }

    // match the open paren
    parser_consume(parser, TOKEN_LPAREN, "expected open paren");
    printf("( ");

    // this loop ignores the "last dot" aspect of R5RS lists
    for (;;) {
        switch (parser->current.type) {
            // an EOF here means we have an umatched paren
            case TOKEN_EOF:
                error(parser, "unmatched paren at EOF");
                return false;
            // an RPAREN ends the current list
            case TOKEN_RPAREN:
                goto done;
            // otherwise we recursively parse the list's children
            default: {
                bool res = print_datum(parser);
                if (!res) return false;
            }
        }
        printf(" ");
    }

done:
    parser_consume(parser, TOKEN_RPAREN, "expected closing paren");
    printf(")");

    return true;
}

static bool
print_abbreviation(struct parser* parser)
{
    // match the quote and then just parse another datum
    parser_consume(parser, TOKEN_QUOTE, "expected quote char");
    printf("'");

    return print_datum(parser);
}

static bool
print_vector(struct parser* parser)
{
    parser_consume(parser, TOKEN_VECTOR, "expected open vector");
    printf("#( ");

    for (;;) {
        switch (parser->current.type) {
            // an EOF here means we have an umatched paren
            case TOKEN_EOF:
                error(parser, "unmatched paren at EOF");
                return false;
            // an RPAREN ends the current list
            case TOKEN_RPAREN:
                goto done;
            // otherwise we recursively parse the vector's children
            default: {
                bool res = print_datum(parser);
                if (!res) return false;
            }
        }
        printf(" ");
    }

done:
    parser_consume(parser, TOKEN_RPAREN, "expected closing paren");
    printf(")");

    return true;
}

bool
parser_print(struct parser* parser)
{
    assert(parser != NULL);

    // init first token and start the recursive parse
    parser_advance(parser);
    while (parser->current.type != TOKEN_EOF) {
        bool res = print_datum(parser);
        printf("\n");

        if (res) {
            printf("syntax ok!\n");
        } else {
            // skip error token and try again
            printf("syntax error!\n");
            parser_advance(parser);
        }
    }

    parser_consume(parser, TOKEN_EOF, "expected end of file");
    return !parser->had_error;
}

static uint8_t
make_constant(struct parser* parser, struct value* value)
{
    long constant = chunk_add_constant(parser->chunk, value);
    if (constant > UINT8_MAX) {
        error(parser, "too many constants in one chunk");
        return 0;
    }

    return (uint8_t)constant;
}

static void
emit_byte(struct parser* parser, uint8_t byte)
{
    chunk_write(parser->chunk, byte, parser->previous.line);
}

static void
emit_bytes(struct parser* parser, uint8_t byte1, uint8_t byte2)
{
    emit_byte(parser, byte1);
    emit_byte(parser, byte2);
}

static void
emit_return(struct parser* parser)
{
    emit_byte(parser, OP_RETURN);
}

static void
emit_constant(struct parser* parser, struct value* value)
{
    uint8_t constant = make_constant(parser, value);
    emit_bytes(parser, OP_CONSTANT, constant);
}

static void
compile_end(struct parser* parser)
{
    emit_return(parser);
}

//static bool compile_datum(struct parser* parser);
//static bool compile_simple_datum(struct parser* parser);
//static bool compile_compound_datum(struct parser* parser);
//static bool compile_list(struct parser* parser);
//static bool compile_abbreviation(struct parser* parser);
//static bool compile_vector(struct parser* parser);

static void compile_number(struct parser* parser);
static void compile_grouping(struct parser* parser);
static void compile_unary(struct parser* parser);
static void compile_expression(struct parser* parser);

// pratt parser table with only prefix parsing funcs (no infix or precendence)
typedef void (*parse_func)(struct parser* parser);
parse_func rules[] = {
    [TOKEN_ERROR]     = NULL, 
    [TOKEN_SYMBOL]    = NULL,
    [TOKEN_BOOLEAN]   = NULL,
    [TOKEN_NUMBER]    = compile_number,
    [TOKEN_CHARACTER] = NULL,
    [TOKEN_STRING]    = NULL,
    [TOKEN_LPAREN]    = compile_grouping,
    [TOKEN_RPAREN]    = NULL,
    [TOKEN_VECTOR]    = NULL,
    [TOKEN_QUOTE]     = NULL,

    [TOKEN_PLUS]      = NULL,
    [TOKEN_MINUS]     = compile_unary,
    [TOKEN_STAR]      = NULL,
    [TOKEN_SLASH]     = NULL,

    [TOKEN_EOF]       = NULL,
};

static void
compile_number(struct parser* parser)
{
    double value = strtod(parser->previous.start, NULL);
    emit_constant(parser, value_make_number(value));
}

static void
compile_grouping(struct parser* parser)
{
    compile_expression(parser);
    parser_consume(parser, TOKEN_RPAREN, "expect ')' after expression");
}

static void
compile_unary(struct parser* parser)
{
    int op_type = parser->previous.type;

    // compile the operand
    compile_expression(parser);

    // emit the operator instruction
    switch (op_type) {
        case TOKEN_MINUS: emit_byte(parser, OP_NEG); break;
        default:
            // unreachable
            return;
    }
}

static void
compile_expression(struct parser* parser)
{
    parser_advance(parser);

    parse_func rule = rules[parser->previous.type];
    if (rule == NULL) {
        error(parser, "expect expression");
        return;
    }

    rule(parser);
}

bool
parser_compile(struct parser* parser, struct chunk* chunk)
{
    assert(parser != NULL);
    assert(chunk != NULL);

    parser->chunk = chunk;

    parser_advance(parser);
    while (!parser_match(parser, TOKEN_EOF)) {
        compile_expression(parser);
    }

    compile_end(parser);
    return !parser->had_error;
}
