#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

void
lexer_init(struct lexer* lexer, const char* source)
{
    assert(lexer != NULL);
    assert(source != NULL);

    lexer->start = source;
    lexer->current = source;
    lexer->line = 1;
}

static struct token
make_token(const struct lexer* lexer, int type)
{
    struct token token = {
        .type = type,
        .start = lexer->start,
        .length = (long)(lexer->current - lexer->start),
        .line = lexer->line,
    };

    return token;
}

static struct token
make_error_token(const struct lexer* lexer, const char* message)
{
    struct token token = {
        .type = TOKEN_ERROR,
        .start = message,
        .length = strlen(message),
        .line = lexer->line,
    };

    return token;
}

static bool
is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static bool
is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool
lexer_eof(const struct lexer* lexer)
{
    return *lexer->current == '\0';
}

static bool
lexer_match(struct lexer* lexer, char expected)
{
    if (lexer_eof(lexer)) return false;
    if (*lexer->current != expected) return false;

    lexer->current++;
    return true;
}

static char
lexer_peek(const struct lexer* lexer)
{
    return *lexer->current;
}

static char
lexer_peek_next(const struct lexer* lexer)
{
    if (lexer_eof(lexer)) return '\0';
    return lexer->current[1];
}

static char
lexer_advance(struct lexer* lexer)
{
    lexer->current++;
    return lexer->current[-1];
}

static void
lexer_skip_whitespace(struct lexer* lexer)
{
    for (;;) {
        char c = lexer_peek(lexer);
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\v':
            case '\f':
                lexer_advance(lexer);
                break;
            case '\n':
                lexer->line++;
                lexer_advance(lexer);
                break;
            case ';':  // if comment is found, advance to newline or EOF
                while (lexer_peek(lexer) != '\n' && !lexer_eof(lexer)) lexer_advance(lexer);
                break;
            default:
                return;
        }
    }
}

static struct token
lexer_next_number(struct lexer* lexer)
{
    // consume digits
    while (is_digit(lexer_peek(lexer))) lexer_advance(lexer);

    // check for and consume fractional part
    if (lexer_peek(lexer) == '.' && is_digit(lexer_peek_next(lexer))) {
        lexer_advance(lexer);
        while (is_digit(lexer_peek(lexer))) lexer_advance(lexer);
    }

    return make_token(lexer, TOKEN_NUMBER);
}

static struct token
lexer_next_identifier(struct lexer* lexer)
{
    // consume valid identifier characters
    while (is_alpha(lexer_peek(lexer)) || is_digit(lexer_peek(lexer))) lexer_advance(lexer);

    return make_token(lexer, TOKEN_IDENTIFIER);
}

static struct token
lexer_next_string(struct lexer* lexer)
{
    while (lexer_peek(lexer) != '"' && !lexer_eof(lexer)) {
        if (lexer_peek(lexer) == '\n') lexer->line++;
        lexer_advance(lexer);
    }

    if (lexer_eof(lexer)) return make_error_token(lexer, "unterminated string");

    // consume closing quote
    lexer_advance(lexer);
    return make_token(lexer, TOKEN_STRING);
}

struct token
lexer_next_token(struct lexer* lexer)
{
    lexer_skip_whitespace(lexer);

    lexer->start = lexer->current;
    if (lexer_eof(lexer)) return make_token(lexer, TOKEN_EOF);

    char c = lexer_advance(lexer);
    if (is_alpha(c)) return lexer_next_identifier(lexer);
    if (is_digit(c)) return lexer_next_number(lexer);

    switch (c) {
        case '(': return make_token(lexer, TOKEN_LPAREN);
        case ')': return make_token(lexer, TOKEN_RPAREN);
        case '\'': return make_token(lexer, TOKEN_QUOTE);
        case '#':
            if (lexer_match(lexer, 't')) return make_token(lexer, TOKEN_BOOLEAN);
            if (lexer_match(lexer, 'f')) return make_token(lexer, TOKEN_BOOLEAN);
            break;
        case '"': return lexer_next_string(lexer);
    }

    return make_error_token(lexer, "unexpected character");
}

const char*
lexer_token_name(int type)
{
    switch (type) {
        case TOKEN_ERROR: return "Error";
        case TOKEN_IDENTIFIER: return "Identifier";
        case TOKEN_BOOLEAN: return "Boolean";
        case TOKEN_NUMBER: return "Number";
        case TOKEN_CHARACTER: return "Character";
        case TOKEN_STRING: return "String";
        case TOKEN_LPAREN: return "LParen";
        case TOKEN_RPAREN: return "RParen";
        case TOKEN_QUOTE: return "Quote";
        case TOKEN_EOF: return "EOF";
        default: return "undefined token";
    }
}
