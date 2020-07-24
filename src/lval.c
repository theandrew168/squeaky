#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lenv.h"
#include "lbuiltin.h"
#include "lval.h"
#include "lval_builtin.h"
#include "lval_error.h"
#include "lval_lambda.h"
#include "lval_list.h"
#include "lval_number.h"
#include "lval_string.h"
#include "lval_symbol.h"
#include "lval_window.h"

static const char SYMBOL_VALID_CHARS[] = 
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789_+-*\\/=<>!&?";

//static const char STRING_ESCAPABLE_CHARS[] =
//    "\a\b\f\n\r\t\v\\\'\"";

static const char STRING_UNESCAPABLE_CHARS[] =
    "abfnrtv\\\'\"";


union lval*
lval_make_error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    union lval* val = malloc(sizeof(union lval));
    lval_error_init(val, fmt, args);

    va_end(args);
    return val;
}

union lval*
lval_make_number(long number)
{
    union lval* val = malloc(sizeof(union lval));
    lval_number_init(val, number);
    return val;
}

union lval*
lval_make_symbol(const char* symbol)
{
    union lval* val = malloc(sizeof(union lval));
    lval_symbol_init(val, symbol);
    return val;
}

union lval*
lval_make_string(const char* string)
{
    union lval* val = malloc(sizeof(union lval));
    lval_string_init(val, string);
    return val;
}

union lval*
lval_make_window(const char* title, long width, long height)
{
    // TODO: check the rc on this
    union lval* val = malloc(sizeof(union lval));
    lval_window_init(val, title, width, height);
    return val;
}

union lval*
lval_make_builtin(lbuiltin builtin)
{
    union lval* val = malloc(sizeof(union lval));
    lval_builtin_init(val, builtin);
    return val;
}

union lval*
lval_make_lambda(union lval* formals, union lval* body)
{
    union lval* val = malloc(sizeof(union lval));
    lval_lambda_init(val, formals, body);
    return val;
}

union lval*
lval_make_sexpr(void)
{
    union lval* val = malloc(sizeof(union lval));
    lval_list_init(val, LVAL_TYPE_SEXPR);
    return val;
}

union lval*
lval_make_qexpr(void)
{
    union lval* val = malloc(sizeof(union lval));
    lval_list_init(val, LVAL_TYPE_QEXPR);
    return val;
}


void
lval_free(union lval* val)
{
    assert(val != NULL);

    // delegate freeing to each type's specific free func
    switch (val->type) {
        case LVAL_TYPE_ERROR: lval_error_free(val); break;
        case LVAL_TYPE_NUMBER: lval_number_free(val); break;
        case LVAL_TYPE_SYMBOL: lval_symbol_free(val); break;
        case LVAL_TYPE_STRING: lval_string_free(val); break;
        case LVAL_TYPE_WINDOW: lval_window_free(val); break;
        case LVAL_TYPE_BUILTIN: lval_builtin_free(val); break;
        case LVAL_TYPE_LAMBDA: lval_lambda_free(val); break;
        case LVAL_TYPE_SEXPR: lval_list_free(val); break;
        case LVAL_TYPE_QEXPR: lval_list_free(val); break;
    }

    // free the base lval itself
    free(val);
}

union lval*
lval_copy(const union lval* val)
{
    assert(val != NULL);

    // alloc the new lval to serve as the copy
    union lval* copy = malloc(sizeof(union lval));

    // delegate copying to each type's specific copy func
    switch (val->type) {
        case LVAL_TYPE_ERROR: lval_error_copy(val, copy); break;
        case LVAL_TYPE_NUMBER: lval_number_copy(val, copy); break;
        case LVAL_TYPE_SYMBOL: lval_symbol_copy(val, copy); break;
        case LVAL_TYPE_STRING: lval_string_copy(val, copy); break;
        case LVAL_TYPE_WINDOW: lval_window_copy(val, copy); break;
        case LVAL_TYPE_BUILTIN: lval_builtin_copy(val, copy); break;
        case LVAL_TYPE_LAMBDA: lval_lambda_copy(val, copy); break;
        case LVAL_TYPE_SEXPR: lval_list_copy(val, copy); break;
        case LVAL_TYPE_QEXPR: lval_list_copy(val, copy); break;
    }

    return copy;
}

const char*
lval_type_name(int lval_type)
{
    switch (lval_type) {
        case LVAL_TYPE_ERROR: return "Error";
        case LVAL_TYPE_NUMBER: return "Number";
        case LVAL_TYPE_SYMBOL: return "Symbol";
        case LVAL_TYPE_STRING: return "String";
        case LVAL_TYPE_WINDOW: return "Window";
        case LVAL_TYPE_BUILTIN: return "Builtin";
        case LVAL_TYPE_LAMBDA: return "Lambda";
        case LVAL_TYPE_SEXPR: return "S-Expression";
        case LVAL_TYPE_QEXPR: return "Q-Expression";
    }

    return "Undefined";
}

bool
lval_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    // diff types are never equal
    if (a->type != b->type) return false;

    // delegate equality check to each type's specific equal func
    switch (a->type) {
        case LVAL_TYPE_ERROR: return lval_error_equal(a, b);
        case LVAL_TYPE_NUMBER: return lval_number_equal(a, b);
        case LVAL_TYPE_SYMBOL: return lval_symbol_equal(a, b);
        case LVAL_TYPE_STRING: return lval_string_equal(a, b);
        case LVAL_TYPE_WINDOW: return lval_window_equal(a, b);
        case LVAL_TYPE_BUILTIN: return lval_builtin_equal(a, b);
        case LVAL_TYPE_LAMBDA: return lval_lambda_equal(a, b);
        case LVAL_TYPE_SEXPR: return lval_list_equal(a, b);
        case LVAL_TYPE_QEXPR: return lval_list_equal(a, b);
    }

    return false;
}

//static const char*
//lval_string_escape(char c)
//{
//    switch (c) {
//        case '\a': return "\\a";
//        case '\b': return "\\b";
//        case '\f': return "\\f";
//        case '\n': return "\\n";
//        case '\r': return "\\r";
//        case '\t': return "\\t";
//        case '\v': return "\\v";
//        case '\\': return "\\\\";
//        case '\'': return "\\\'";
//        case '\"': return "\\\"";
//        default: return "";
//    }
//}


static char
lval_string_unescape(char c)
{
    switch (c) {
        case 'a': return '\a';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'v': return '\v';
        case '\\': return '\\';
        case '\'': return '\'';
        case '\"': return '\"';
        default: return '\0';
    }
}

void
lval_print(const union lval* val)
{
    assert(val != NULL);

    // delegate printing to each type's specific print func
    switch (val->type) {
        case LVAL_TYPE_ERROR: lval_error_print(val); break;
        case LVAL_TYPE_NUMBER: lval_number_print(val); break;
        case LVAL_TYPE_SYMBOL: lval_symbol_print(val); break;
        case LVAL_TYPE_STRING: lval_string_print(val); break;
        case LVAL_TYPE_WINDOW: lval_window_print(val); break;
        case LVAL_TYPE_BUILTIN: lval_builtin_print(val); break;
        case LVAL_TYPE_LAMBDA: lval_lambda_print(val); break;
        case LVAL_TYPE_SEXPR: lval_list_print(val, '(', ')'); break;
        case LVAL_TYPE_QEXPR: lval_list_print(val, '{', '}'); break;
        default: printf("!!! undefined lisp value !!!"); break;
    }
}

void
lval_println(const union lval* val)
{
    assert(val != NULL);

    // defer to lval_print and add a newline
    lval_print(val);
    putchar('\n');
}


static union lval*
lval_read_symbol(char* s, long* i)
{
    // alloc an empty string
    char* part = malloc(1 * sizeof(char));
    *part = '\0';

    // while valid symbol chars
    while (strchr(SYMBOL_VALID_CHARS, s[*i]) && s[*i] != '\0') {
        // append char to end of string
        part = realloc(part, strlen(part) + 2);
        part[strlen(part) + 1] = '\0';
        part[strlen(part) + 0] = s[*i];
        (*i)++;
    }

    // check if symbol looks like a number
    bool is_num = strchr("-0123456789", part[0]) != NULL;
    for (long j = 1; j < (long)strlen(part); j++) {
        if (strchr("0123456789", part[j]) == NULL) {
            is_num = false;
            break;
        }
    }
    if (strlen(part) == 1 && part[0] == '-') is_num = false;

    // create lval as either number or symbol
    union lval* x = NULL;
    if (is_num) {
        errno = 0;
        long v = strtol(part, NULL, 10);
        x = (errno != ERANGE) ? lval_make_number(v) : lval_make_error("invalid number: %s", part);
    } else {
        x = lval_make_symbol(part);
    }

    free(part);
    return x;
}

static union lval*
lval_read_string(char* s, long* i)
{
    // alloc an empty string
    char* part = malloc(1 * sizeof(char));
    *part = '\0';

    (*i)++;
    while (s[*i] != '"') {
        char c = s[*i];

        // if EOI then there is an incomplete string literal
        if (c == '\0') {
            free(part);
            return lval_make_error("unexpected end of input");
        }

        // if backslash then unescape the following char
        if (c == '\\') {
            (*i)++;
            if (strchr(STRING_UNESCAPABLE_CHARS, s[*i])) {
                c = lval_string_unescape(s[*i]);
            } else {
                free(part);
                return lval_make_error("invalid escape sequence: \\%c", s[*i]);
            }
        }

        // append char to end of string
        part = realloc(part, strlen(part) + 2);
        part[strlen(part) + 1] = '\0';
        part[strlen(part) + 0] = c;
        (*i)++;
    }

    // move forward past final quote char
    (*i)++;

    union lval* x = lval_make_string(part);
    free(part);
    return x;
}

static union lval*
lval_read(char* s, long* i)
{
    // skip whitespace and comments
    while (strchr(" \t\v\r\n;", s[*i]) && s[*i] != '\0') {
        if (s[*i] == ';') {
            while (s[*i] != '\n' && s[*i] != '\0') (*i)++;
        }
        (*i)++;
    }

    union lval* x = NULL;

    if (s[*i] == '\0') {
        // if we reach EOI then we're missing something
        return lval_make_error("unexpected end of input");
    } else if (s[*i] == '(') {
        // if next char is ( then read s-expr
        (*i)++;
        x = lval_read_expr(s, i, ')');
    } else if (s[*i] == '{') {
        // if next char is { then read q-expr
        (*i)++;
        x = lval_read_expr(s, i, '}');
    } else if (strchr(SYMBOL_VALID_CHARS, s[*i])) {
        // if next char is part of a symbol then read a symbol
        x = lval_read_symbol(s, i);
    } else if (strchr("\"", s[*i])) {
        // if next char is dub quote then read a string
        x = lval_read_string(s, i);
    } else {
        x = lval_make_error("unexpected character: %c", s[*i]);
    }

    while (strchr(" \t\v\r\n;", s[*i]) && s[*i] != '\0') {
        if (s[*i] == ';') {
            while (s[*i] != '\n' && s[*i] != '\0') (*i)++;
        }
        (*i)++;
    }

    return x;
}

union lval*
lval_read_expr(char* s, long* i, char end)
{
    union lval* x = (end == '}') ? lval_make_qexpr() : lval_make_sexpr();

    while (s[*i] != end) {
        union lval* y = lval_read(s, i);
        if (y->type == LVAL_TYPE_ERROR) {
            lval_free(x);
            return y;
        } else {
            lval_list_append(x, y);
        }
    }

    (*i)++;
    return x;
}
