#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

    parser->lexer = lexer;
}

static void
next_token(struct parser* parser)
{
    parser->current_token = lexer_next_token(parser->lexer);
}

static int
match_token(struct parser* parser, int type)
{
    if (parser->current_token.type != type) {
        return PARSER_ERROR;
    }

    next_token(parser);
    return PARSER_OK;
}

static int parse_datum(struct parser* parser);
static int parse_simple_datum(struct parser* parser);
static int parse_compound_datum(struct parser* parser);
static int parse_list(struct parser* parser);
static int parse_abbreviation(struct parser* parser);
static int parse_vector(struct parser* parser);

static int
parse_datum(struct parser* parser)
{
    switch (parser->current_token.type) {
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
            fprintf(stderr, "invalid datum: %s\n", lexer_token_name(parser->current_token.type));
            return PARSER_ERROR;
    }
}

static int
parse_simple_datum(struct parser* parser)
{
    switch (parser->current_token.type) {
        case TOKEN_BOOLEAN:
            printf("%s", lexer_token_name(TOKEN_BOOLEAN));
            break;
        case TOKEN_CHARACTER:
            printf("%s", lexer_token_name(TOKEN_CHARACTER));
            break;
        case TOKEN_NUMBER:
            printf("%s", lexer_token_name(TOKEN_NUMBER));
            break;
        case TOKEN_SYMBOL:
            printf("%s", lexer_token_name(TOKEN_SYMBOL));
            break;
        case TOKEN_STRING:
            printf("%s", lexer_token_name(TOKEN_STRING));
            break;
        default:
            fprintf(stderr, "invalid simple datum: %s\n", lexer_token_name(parser->current_token.type));
            return PARSER_ERROR;
    }

    next_token(parser);
    return PARSER_OK;
}

static int
parse_compound_datum(struct parser* parser)
{
    switch (parser->current_token.type) {
        case TOKEN_LPAREN: 
        case TOKEN_QUOTE:
            return parse_list(parser);
        case TOKEN_VECTOR:
            return parse_vector(parser);
        default:
            fprintf(stderr, "invalid compound datum: %s\n", lexer_token_name(parser->current_token.type));
            return PARSER_ERROR;
    }
}

static int
parse_list(struct parser* parser)
{
    // check for quote and parse abbrev if found
    if (parser->current_token.type == TOKEN_QUOTE) {
        return parse_abbreviation(parser);
    }

    match_token(parser, TOKEN_LPAREN);
    printf("( ");

    // this loop ignores the "last dot" aspect of R5RS lists
    for (;;) {
        switch (parser->current_token.type) {
            // an EOF here means we have an umatched paren
            case TOKEN_EOF:
                fprintf(stderr, "unmatched paren at EOF\n");
                return PARSER_ERROR;
            // an RPAREN ends the current list
            case TOKEN_RPAREN:
                goto done;
            // otherwise we recursively parse the list's children
            default: {
                int rc = parse_datum(parser);
                if (rc != PARSER_OK) return rc;
            }
        }
        printf(" ");
    }

done:
    match_token(parser, TOKEN_RPAREN);
    printf(")");

    return PARSER_OK;
}

static int
parse_abbreviation(struct parser* parser)
{
    // match the quote and then just parse another datum
    match_token(parser, TOKEN_QUOTE);
    printf("'");

    int rc = parse_datum(parser);
    if (rc != PARSER_OK) return rc;

    return PARSER_OK;
}

static int
parse_vector(struct parser* parser)
{
    match_token(parser, TOKEN_VECTOR);
    printf("#( ");

    for (;;) {
        switch (parser->current_token.type) {
            // an EOF here means we have an umatched paren
            case TOKEN_EOF:
                fprintf(stderr, "unmatched paren at EOF\n");
                return PARSER_ERROR;
            // an RPAREN ends the current list
            case TOKEN_RPAREN:
                goto done;
            // otherwise we recursively parse the vector's children
            default: {
                int rc = parse_datum(parser);
                if (rc != PARSER_OK) return rc;
            }
        }
        printf(" ");
    }

done:
    match_token(parser, TOKEN_RPAREN);
    printf(")");

    return PARSER_OK;
}

int
parser_print_ast(struct parser* parser)
{
    assert(parser != NULL);

    // init first token and start the recursive parse
    next_token(parser);
    while (parser->current_token.type != TOKEN_EOF) {
        int rc = parse_datum(parser);
        printf("\n");
        if (rc != PARSER_OK) {
            printf("syntax error!\n");
            break;
        } else {
            printf("syntax ok!\n");
        }
    }

    return PARSER_OK;
}
