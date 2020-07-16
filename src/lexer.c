#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static const char*
token_type_name(int type)
{
    switch (type) {
    case TOKEN_TYPE_FIXNUM:
        return "FIXNUM";
    case TOKEN_TYPE_BOOLEAN:
        return "BOOLEAN";
    case TOKEN_TYPE_CHARACTER:
        return "CHARACTER";
    case TOKEN_TYPE_UNDEFINED:
    default:
        return NULL;
    }
}

static char
lexer_next(struct lexer* lexer)
{
    if (lexer->pos >= lexer->input_len) {
        lexer->width = 0;
        return -1;
    }

    char c = lexer->input[lexer->pos];
    lexer->width = 1;  // TODO: utf8 stuff will change return type
    lexer->pos += lexer->width;
    return c;
}

static void
lexer_ignore(struct lexer* lexer)
{
    lexer->start = lexer->pos;
}

static void
lexer_backup(struct lexer* lexer)
{
    lexer->pos -= lexer->width;
}

static char
lexer_peek(struct lexer* lexer)
{
    char c = lexer_next(lexer);
    lexer_backup(lexer);
    return c;
}

static bool
lexer_accept(struct lexer* lexer, const char* valid)
{
    if (strchr(valid, lexer_next(lexer)) != NULL) {
        return true;
    }
    lexer_backup(lexer);
    return false;
}

static void
lexer_accept_run(struct lexer* lexer, const char* valid)
{
    while (strchr(valid, lexer_next(lexer)) != NULL);
    lexer_backup(lexer);
}

static void
lexer_emit(struct lexer* lexer, struct token* token, enum token_type type)
{
    token->type = type;
    token->value = lexer->input + lexer->start;
    token->len = lexer->pos - lexer->start;
    lexer->start = lexer->pos;
}

static void
lexer_lex_fixnum(struct lexer* lexer, struct token* token)
{
    const char digits[] = "012345679";
    lexer_accept_run(lexer, digits);
    if (lexer_accept(lexer, ".")) {
        lexer_accept_run(lexer, digits);
    }

    lexer_emit(lexer, token, TOKEN_TYPE_FIXNUM);
}

void
lexer_print(const struct token* token)
{
    // TODO: check len and truncate extra long tokens
    printf("%s: %.*s\n", token_type_name(token->type), (int)token->len, token->value);
}

int
lexer_lex(struct lexer* lexer, struct token* token)
{
    while (lexer_accept(lexer, " \t\r\n")) {
        lexer_ignore(lexer);
    }

    if (lexer_accept(lexer, "0123456789")) {
        lexer_lex_fixnum(lexer, token);
        return LEXER_OK;
    } else {
        return LEXER_ERROR;
    }
}
