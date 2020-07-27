#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

static bool parse_datum(struct parser* parser);
static bool parse_simple_datum(struct parser* parser);
static bool parse_compound_datum(struct parser* parser);
static bool parse_list(struct parser* parser);
static bool parse_abbreviation(struct parser* parser);
static bool parse_vector(struct parser* parser);

static bool
parse_datum(struct parser* parser)
{
    switch (parser->current.type) {
        case TOKEN_BOOLEAN:
        case TOKEN_NUMBER:
        case TOKEN_CHARACTER:
        case TOKEN_STRING:
        case TOKEN_SYMBOL:
            return parse_simple_datum(parser);
        case TOKEN_LPAREN: 
        case TOKEN_VECTOR:
            return parse_compound_datum(parser);
        default: 
            error(parser, "invalid datum");
            return false;
    }
}

static bool
parse_simple_datum(struct parser* parser)
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
parse_compound_datum(struct parser* parser)
{
    switch (parser->current.type) {
        case TOKEN_LPAREN: 
        case TOKEN_QUOTE:
            return parse_list(parser);
        case TOKEN_VECTOR:
            return parse_vector(parser);
        default:
            error(parser, "invalid compound datum");
            return false;
    }
}

static bool
parse_list(struct parser* parser)
{
    // check for quote and parse abbrev if found
    if (parser->current.type == TOKEN_QUOTE) {
        return parse_abbreviation(parser);
    }

    // consume the open paren
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
                bool res = parse_datum(parser);
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
parse_abbreviation(struct parser* parser)
{
    // match the quote and then just parse another datum
    parser_consume(parser, TOKEN_QUOTE, "expected quote char");
    printf("'");

    return parse_datum(parser);
}

static bool
parse_vector(struct parser* parser)
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
                bool res = parse_datum(parser);
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
parser_compile(struct parser* parser, struct chunk* chunk)
{
    assert(parser != NULL);
    assert(chunk != NULL);

    parser->chunk = chunk;
    return true;
}

bool
parser_print_ast(struct parser* parser)
{
    assert(parser != NULL);

    // init first token and start the recursive parse
    parser_advance(parser);
    while (parser->current.type != TOKEN_EOF) {
        bool res = parse_datum(parser);
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
