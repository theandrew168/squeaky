#ifndef SQUEAKY_LEXER_H_INCLUDED
#define SQUEAKY_LEXER_H_INCLUDED

enum token_type {
    TOKEN_ERROR = 0,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_SYMBOL,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_QUOTE,
    TOKEN_EOF,
};

struct token {
    int type;
    const char* start;
    long length;
    long line;
};

void lexer_token_print(const struct token* token);
void lexer_token_println(const struct token* token);
const char* lexer_token_name(int type);

struct lexer {
    const char* start;
    const char* current;
    long line;
};

void lexer_init(struct lexer* lexer, const char* source);
struct token lexer_next_token(struct lexer* lexer);

#endif
