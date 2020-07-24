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
lval_string_init(union lval* val, const char* string)
{
    assert(val != NULL);
    assert(string != NULL);

    struct lval_string* v = AS_STRING(val);

    v->type = LVAL_TYPE_STRING;
    v->string = malloc(strlen(string) + 1);
    strcpy(v->string, string);

    return true;
}

void
lval_string_free(union lval* val)
{
    assert(val != NULL);

    struct lval_string* v = AS_STRING(val);

    free(v->string);
}

void
lval_string_copy(const union lval* val, union lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    const struct lval_string* v = AS_CONST_STRING(val);
    struct lval_string* c = AS_STRING(copy);

    c->type = v->type;
    c->string = malloc(strlen(v->string) + 1);
    strcpy(c->string, v->string);
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
lval_string_print(const union lval* val)
{
    assert(val != NULL);

    const struct lval_string* v = AS_CONST_STRING(val);

//    putchar('"');
    for (long i = 0; i < (long)strlen(v->string); i++) {
        // escape chars for printing when necessary
        if (strchr(ESCAPABLE_CHARS, v->string[i])) {
            printf("%s", escape(v->string[i]));
        } else {
            putchar(v->string[i]);
        }
    }
//    putchar('"');
}

bool
lval_string_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    const struct lval_string* aa = AS_CONST_STRING(a);
    const struct lval_string* bb = AS_CONST_STRING(b);

    return strcmp(aa->string, bb->string) == 0;
}
