#ifndef SQUEAKY_PARSER_H_INCLUDED
#define SQUEAKY_PARSER_H_INCLUDED

#include <stdbool.h>

#include "lexer.h"
#include "value.h"

struct parser {
    struct lexer* lexer;
    struct token current;
    struct token previous;
    bool had_error;
    bool panic_mode;
};

void parser_init(struct parser* parser, struct lexer* lexer);
bool parser_print(struct parser* parser);
struct value* parser_parse(struct parser* parser);

#endif
