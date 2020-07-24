#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"
#include "lval_string.h"

static const char ESCAPABLE_CHARS[] = "\a\b\f\n\r\t\v\\\'\"";
//static const char UNESCAPABLE_CHARS[] = "abfnrtv\\\'\"";

bool
lval_string_init(struct lval* val, const char* string)
{
    assert(val != NULL);
    assert(string != NULL);

    val->type = LVAL_TYPE_STRING;
    val->as.string = malloc(strlen(string) + 1);
    strcpy(val->as.string, string);

    return true;
}

void
lval_string_free(struct lval* val)
{
    assert(val != NULL);

    free(val->as.string);
}

void
lval_string_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    copy->as.string = malloc(strlen(val->as.string) + 1);
    strcpy(copy->as.string, val->as.string);
}

static const char*
escape(char c)
{
    switch (c) {
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        case '\\': return "\\\\";
        case '\'': return "\\\'";
        case '\"': return "\\\"";
        default: return "";
    }
}

// TODO: used in reading
//static char
//unescape(char c)
//{
//    switch (c) {
//        case 'a': return '\a';
//        case 'b': return '\b';
//        case 'f': return '\f';
//        case 'n': return '\n';
//        case 'r': return '\r';
//        case 't': return '\t';
//        case 'v': return '\v';
//        case '\\': return '\\';
//        case '\'': return '\'';
//        case '\"': return '\"';
//        default: return '\0';
//    }
//}

void
lval_string_print(const struct lval* val)
{
    assert(val != NULL);

//    putchar('"');
    for (long i = 0; i < (long)strlen(val->as.string); i++) {
        // escape chars for printing when necessary
        if (strchr(ESCAPABLE_CHARS, val->as.string[i])) {
            printf("%s", escape(val->as.string[i]));
        } else {
            putchar(val->as.string[i]);
        }
    }
//    putchar('"');
}

bool
lval_string_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return strcmp(a->as.string, b->as.string) == 0;
}
