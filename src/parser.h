#ifndef SQUEAKY_PARSER_H_INCLUDED
#define SQUEAKY_PARSER_H_INCLUDED

#include "chunk.h"
#include "lexer.h"

struct parser {
    struct lexer* lexer;
    struct token current_token;
};

enum parser_status {
    PARSER_OK = 0,
    PARSER_ERROR,
};

void parser_init(struct parser* parser, struct lexer* lexer);

int parser_print_ast(struct parser* parser);

#endif
