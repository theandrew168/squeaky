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
    AS_STRING(val)->string = malloc(strlen(string) + 1);
    strcpy(AS_STRING(val)->string, string);

    return true;
}

void
lval_string_free(struct lval* val)
{
    assert(val != NULL);

    free(AS_STRING(val)->string);
}

void
lval_string_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    AS_STRING(copy)->string = malloc(strlen(AS_STRING(val)->string) + 1);
    strcpy(AS_STRING(copy)->string, AS_STRING(val)->string);
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
    for (long i = 0; i < (long)strlen(AS_STRING(val)->string); i++) {
        // escape chars for printing when necessary
        if (strchr(ESCAPABLE_CHARS, AS_STRING(val)->string[i])) {
            printf("%s", escape(AS_STRING(val)->string[i]));
        } else {
            putchar(AS_STRING(val)->string[i]);
        }
    }
//    putchar('"');
}

bool
lval_string_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return strcmp(AS_STRING(a)->string, AS_STRING(b)->string) == 0;
}
