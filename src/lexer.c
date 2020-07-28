#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"

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
// <special_initial>     : /[!$%&*/:<=>?^_~]/
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

// TODO: should these operate on tokens?

static bool
is_letter(char c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z');
}

static bool
is_special_initial(char c)
{
    return strchr("!$%&*/:<=>?^_~", c) != NULL;
}

static bool
is_special_subsequent(char c)
{
    return strchr("+-.@", c) != NULL;
}

static bool
is_special_idenfitier(const char* s)
{
    return strcmp("+", s) == 0 ||
           strcmp("-", s) == 0 ||
           strcmp("...", s) == 0;
}

static bool
is_initial(char c)
{
    return is_letter(c) || is_special_initial(c);
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

void
lexer_token_print(const struct token* token)
{
    assert(token != NULL);

    printf("  %-10s %.*s", lexer_token_name(token->type), (int)token->length, token->start);
}

void
lexer_token_println(const struct token* token)
{
    assert(token != NULL);

    lexer_token_print(token);
    printf("\n");
}

const char*
lexer_token_name(int type)
{
    switch (type) {
        case TOKEN_ERROR: return "Error";
        case TOKEN_NUMBER: return "Number";
        case TOKEN_STRING: return "String";
        case TOKEN_SYMBOL: return "Symbol";
        case TOKEN_LPAREN: return "LParen";
        case TOKEN_RPAREN: return "RParen";
        case TOKEN_QUOTE: return "Quote";
        case TOKEN_EOF: return "EOF";
        default: return "undefined";
    }
}

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
lexer_eof(const struct lexer* lexer)
{
    return lexer->current[0] == '\0';
}

static char
lexer_peek(const struct lexer* lexer)
{
    return lexer->current[0];
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

static bool
lexer_match(struct lexer* lexer, char expected)
{
    if (lexer_eof(lexer)) return false;
    if (lexer->current[0] != expected) return false;

    lexer_advance(lexer);
    return true;
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
lexer_next_symbol(struct lexer* lexer)
{
    // consume valid identifier characters
    while (is_alpha(lexer_peek(lexer)) || is_digit(lexer_peek(lexer))) lexer_advance(lexer);

    return make_token(lexer, TOKEN_SYMBOL);
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

    // advance start pointer to the end of the previous token
    lexer->start = lexer->current;
    if (lexer_eof(lexer)) return make_token(lexer, TOKEN_EOF);

    char c = lexer_advance(lexer);
    if (is_alpha(c)) return lexer_next_symbol(lexer);
    if (is_digit(c)) return lexer_next_number(lexer);

    switch (c) {
        case '(': return make_token(lexer, TOKEN_LPAREN);
        case ')': return make_token(lexer, TOKEN_RPAREN);
        case '\'': return make_token(lexer, TOKEN_QUOTE);
        case '"': return lexer_next_string(lexer);
    }

    return make_error_token(lexer, "unexpected character");
}
