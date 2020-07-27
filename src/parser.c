#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

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
static int parse_list(struct parser* parser);
static int parse_abbreviation(struct parser* parser);
static int parse_simple_datum(struct parser* parser);

static int
parse_datum(struct parser* parser)
{
    switch (parser->current_token.type) {
        case TOKEN_LPAREN: return parse_list(parser);
        case TOKEN_QUOTE: return parse_abbreviation(parser);
        default: return parse_simple_datum(parser);
    }
}

static int
parse_list(struct parser* parser)
{
    match_token(parser, TOKEN_LPAREN);
    printf("( ");

    for (;;) {
        switch (parser->current_token.type) {
            case TOKEN_EOF:
                fprintf(stderr, "unmatched paren at EOF\n");
                return PARSER_ERROR;
            case TOKEN_RPAREN:
                goto done;
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
    match_token(parser, TOKEN_QUOTE);
    printf("'");

    int rc = parse_datum(parser);
    if (rc != PARSER_OK) return rc;

    return PARSER_OK;
}

static int
parse_simple_datum(struct parser* parser)
{
    switch (parser->current_token.type) {
        case TOKEN_BOOLEAN:
            printf("%s", lexer_token_name(TOKEN_BOOLEAN));
            break;
        case TOKEN_NUMBER:
            printf("%s", lexer_token_name(TOKEN_NUMBER));
            break;
        case TOKEN_IDENTIFIER:
            printf("%s", lexer_token_name(TOKEN_IDENTIFIER));
            break;
        case TOKEN_STRING:
            printf("%s", lexer_token_name(TOKEN_STRING));
            break;
        default:
            fprintf(stderr, "invalid simple datum\n");
            return PARSER_ERROR;
    }

    next_token(parser);
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
