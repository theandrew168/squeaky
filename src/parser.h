#ifndef SQUEAKY_PARSER_H_INCLUDED
#define SQUEAKY_PARSER_H_INCLUDED

#include <stdbool.h>

#include "chunk.h"
#include "lexer.h"

struct parser {
    struct chunk* chunk;
    struct lexer* lexer;
    struct token current;
    struct token previous;
    bool had_error;
    bool panic_mode;
};

void parser_init(struct parser* parser, struct lexer* lexer);

bool parser_compile(struct parser* parser, struct chunk* chunk);
bool parser_print_ast(struct parser* parser);

#endif
