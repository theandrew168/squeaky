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
    case TOKEN_TYPE_STRING:
        return "STRING";
    case TOKEN_TYPE_UNDEFINED:
    default:
        return "UNDEFINED";
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
lexer_match(struct lexer* lexer, const char* str)
{
    long pos = lexer->pos;
    while (*str != '\0') {
        if (lexer_next(lexer) != *str) {
            lexer->pos = pos;
            return false;
        }
        str++;
    }
    return true;
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

static int
lexer_lex_fixnum(struct lexer* lexer, struct token* token)
{
    const char digits[] = "012345679";
    lexer_accept_run(lexer, digits);
    if (lexer_accept(lexer, ".")) {
        lexer_accept_run(lexer, digits);
    }

    lexer_emit(lexer, token, TOKEN_TYPE_FIXNUM);
    return LEXER_OK;
}

static int
lexer_lex_boolean(struct lexer* lexer, struct token* token)
{
    lexer_emit(lexer, token, TOKEN_TYPE_BOOLEAN);
    return LEXER_OK;
}

static int
lexer_lex_character(struct lexer* lexer, struct token* token)
{
    if (lexer_match(lexer, "newline") || lexer_match(lexer, "space")) {
        lexer_emit(lexer, token, TOKEN_TYPE_CHARACTER);
        return LEXER_OK;
    }

    const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    if (!lexer_accept(lexer, chars)) {
        return LEXER_ERROR;
    }

    lexer_emit(lexer, token, TOKEN_TYPE_CHARACTER);
    return LEXER_OK;
}

static int
lexer_lex_string(struct lexer* lexer, struct token* token)
{
    for (;;) {
        char next = lexer_peek(lexer);
        if (next == -1) {
//            fprintf(stderr, "incomplete string literal\n");
            return LEXER_ERROR;
        }

        lexer_next(lexer);

        // use a cheeky lookahead(2) to accept escapes quotes
        if (next == '\\' && lexer_peek(lexer) == '"') lexer_next(lexer);

        if (next == '"') break;
    }

    lexer_emit(lexer, token, TOKEN_TYPE_STRING);
    return LEXER_OK;
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
    // ignore whitespace and comments lines
    while (lexer_accept(lexer, " \t\r\n")) {
        lexer_ignore(lexer);
        if (lexer_accept(lexer, ";")) {
            lexer_ignore(lexer);
            for (;;) {
                if (lexer_peek(lexer) == '\n' || lexer_peek(lexer) == -1) break;
                lexer_next(lexer);
                lexer_ignore(lexer);
            }
        }
    }

    if (lexer_accept(lexer, "0123456789")) {
        return lexer_lex_fixnum(lexer, token);
    }

    if (lexer_accept(lexer, "#")) {
        if (lexer_accept(lexer, "tf")) {
            return lexer_lex_boolean(lexer, token);
        }

        if (lexer_accept(lexer, "\\")) {
            return lexer_lex_character(lexer, token);
        }

        return LEXER_ERROR;
    }

    if (lexer_accept(lexer, "\"")) {
        return lexer_lex_string(lexer, token);
    }


    return LEXER_ERROR;
}
