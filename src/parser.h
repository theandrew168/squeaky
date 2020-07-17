#ifndef SQUEAKY_PARSER_H_INCLUDED
#define SQUEAKY_PARSER_H_INCLUDED

#include <stdbool.h>

#include "lexer.h"

enum ast_type {
    AST_UNDEFINED = 0,
    AST_FIXNUM,
    AST_BOOLEAN,
    AST_CHARACTER,
    AST_STRING,
    AST_SEXPR,
};

struct ast {
    int type;
    union {
        struct {
            long value;
        } fixnum;
        struct {
            bool value;
        } boolean;
        struct {
            char value;
        } character;
        struct {
            char* value;
        } string;
        struct {
            struct ast* list;
            long count;
        } sexpr;
    } data;
};

enum parser_status {
    PARSER_OK = 0,
    PARSER_ERROR,
};

int parser_parse(const struct token* tokens, long count);

#endif
