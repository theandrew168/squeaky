#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

//  number : /-?[0-9]+/
//  symbol : '+' | '-' | '*' | '/'
//  sexpr  : '(' <expr>* ')'
//  expr   : <number> | <symbol> | <sexpr>
//  lispy  : /^/ <expr>* /$/

static int
parser_parse_expr(const struct token* tokens, long count)
{
    assert(tokens != NULL);

    struct token token = *tokens;
    if (token.type == TOKEN_TYPE_FIXNUM) {
        char* str = malloc(token.len + 1);
        memmove(str, token.value, token.len);
        str[token.len] = '\0';

        long value = strtol(str, NULL, 10);

        free(str);

        printf("%ld\n", value);
        return PARSER_OK;
    }

    return PARSER_ERROR;
}

int
parser_parse(const struct token* tokens, long count)
{
    assert(tokens != NULL);

    return parser_parse_expr(tokens, count);
}
