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
lval_string_init(struct lval_string* val, const char* string)
{
    assert(val != NULL);
    assert(string != NULL);

    val->type = LVAL_TYPE_STRING;
    val->string = malloc(strlen(string) + 1);
    strcpy(val->string, string);
    return true;
}

void
lval_string_free(struct lval_string* val)
{
    assert(val != NULL);

    free(val->string);
}

void
lval_string_copy(const struct lval_string* val, struct lval_string* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->string = malloc(strlen(val->string) + 1);
    strcpy(copy->string, val->string);
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
lval_string_print(const struct lval_string* val)
{
    assert(val != NULL);

//    putchar('"');
    for (long i = 0; i < (long)strlen(val->string); i++) {
        // escape chars for printing when necessary
        if (strchr(ESCAPABLE_CHARS, val->string[i])) {
            printf("%s", escape(val->string[i]));
        } else {
            putchar(val->string[i]);
        }
    }
//    putchar('"');
}

bool
lval_string_equal(const struct lval_string* a, const struct lval_string* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return strcmp(a->string, b->string) == 0;
}
