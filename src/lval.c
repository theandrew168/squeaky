#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lenv.h"
#include "lval.h"

static const char SYMBOL_VALID_CHARS[] = 
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789_+-*\\/=<>!&?";

static const char STRING_ESCAPABLE_CHARS[] =
    "\a\b\f\n\r\t\v\\\'\"";

static const char STRING_UNESCAPABLE_CHARS[] =
    "abfnrtv\\\'\"";

struct lval*
lval_make_error(const char* fmt, ...)
{
    struct lval* val = malloc(sizeof(struct lval));
    val->type = LVAL_TYPE_ERROR;

    va_list va;
    va_start(va, fmt);

    val->error = malloc(512);
    vsnprintf(val->error, 511, fmt, va);
    val->error = realloc(val->error, strlen(val->error) + 1);

    va_end(va);
    return val;
}

struct lval*
lval_make_number(long number)
{
    struct lval* val = malloc(sizeof(struct lval));
    val->type = LVAL_TYPE_NUMBER;
    val->number = number;
    return val;
}

struct lval*
lval_make_symbol(const char* symbol)
{
    struct lval* val = malloc(sizeof(struct lval));
    val->type = LVAL_TYPE_SYMBOL;
    val->symbol = malloc(strlen(symbol) + 1);
    strcpy(val->symbol, symbol);
    return val;
}

struct lval*
lval_make_string(const char* string)
{
    struct lval* val = malloc(sizeof(struct lval));
    val->type = LVAL_TYPE_STRING;
    val->string = malloc(strlen(string) + 1);
    strcpy(val->string, string);
    return val;
}

struct lval*
lval_make_builtin(lbuiltin builtin)
{
    struct lval* val = malloc(sizeof(struct lval));
    val->type = LVAL_TYPE_FUNC;
    val->builtin = builtin;
    return val;
}

struct lval*
lval_make_lambda(struct lval* formals, struct lval* body)
{
    struct lval* val = malloc(sizeof(struct lval));
    val->type = LVAL_TYPE_FUNC;

    val->builtin = NULL;

    val->env = lenv_make();
    val->formals = formals;
    val->body = body;

    return val;
}

struct lval*
lval_make_sexpr(void)
{
    struct lval* val = malloc(sizeof(struct lval));
    val->type = LVAL_TYPE_SEXPR;
    val->cell_count = 0;
    val->cell = NULL;
    return val;
}

struct lval*
lval_make_qexpr(void)
{
    struct lval* val = malloc(sizeof(struct lval));
    val->type = LVAL_TYPE_QEXPR;
    val->cell_count = 0;
    val->cell = NULL;
    return val;
}

struct lval*
lval_copy(const struct lval* val)
{
    assert(val != NULL);

    struct lval* copy = malloc(sizeof(struct lval));
    copy->type = val->type;

    switch (val->type) {
        case LVAL_TYPE_ERROR:
            copy->error = malloc(strlen(val->error) + 1);
            strcpy(copy->error, val->error);
            break;
        case LVAL_TYPE_NUMBER:
            copy->number = val->number;
            break;
        case LVAL_TYPE_SYMBOL:
            copy->symbol = malloc(strlen(val->symbol) + 1);
            strcpy(copy->symbol, val->symbol);
            break;
        case LVAL_TYPE_STRING:
            copy->string = malloc(strlen(val->string) + 1);
            strcpy(copy->string, val->string);
            break;
        case LVAL_TYPE_FUNC:
            if (val->builtin != NULL) {
                copy->builtin = val->builtin;
            } else {
                copy->builtin = NULL;
                copy->env = lenv_copy(val->env);
                copy->formals = lval_copy(val->formals);
                copy->body = lval_copy(val->body);
            }
            break;
        case LVAL_TYPE_SEXPR:
        case LVAL_TYPE_QEXPR:
            copy->cell_count = val->cell_count;
            copy->cell = malloc(sizeof(struct lval*) * val->cell_count);
            for (long i = 0; i < val->cell_count; i++) {
                copy->cell[i] = lval_copy(val->cell[i]);
            }
            break;
    }

    return copy;
}

void
lval_free(struct lval* val)
{
    assert(val != NULL);

    if (val->type == LVAL_TYPE_ERROR) free(val->error);
    if (val->type == LVAL_TYPE_SYMBOL) free(val->symbol);
    if (val->type == LVAL_TYPE_STRING) free(val->string);
    if (val->type == LVAL_TYPE_FUNC && val->builtin == NULL) {
        lenv_free(val->env);
        lval_free(val->formals);
        lval_free(val->body);
    }
    if (val->type == LVAL_TYPE_SEXPR || val->type == LVAL_TYPE_QEXPR) {
        for (long i = 0; i < val->cell_count; i++) {
            lval_free(val->cell[i]);
        }
        free(val->cell);
    }

    free(val);
}

bool
lval_eq(const struct lval* x, const struct lval* y)
{
    assert(x != NULL);
    assert(y != NULL);

    // diff types are never equal
    if (x->type != y->type) return false;

    // compare based on the type
    switch (x->type) {
        case LVAL_TYPE_ERROR:
            return strcmp(x->error, y->error) == 0;
        case LVAL_TYPE_NUMBER:
            return x->number == y->number;
        case LVAL_TYPE_SYMBOL:
            return strcmp(x->symbol, y->symbol) == 0;
        case LVAL_TYPE_STRING:
            return strcmp(x->string, y->string) == 0;
        case LVAL_TYPE_FUNC:
            if (x->builtin || y->builtin) return x->builtin == y->builtin;
            else return lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body);
        case LVAL_TYPE_SEXPR:
        case LVAL_TYPE_QEXPR:
            if (x->cell_count != y->cell_count) return false;
            for (long i = 0; i < x->cell_count; i++) {
                if (!lval_eq(x->cell[i], y->cell[i])) return false;
            }
            return true;
        default:
            return false;
    }
}

const char*
lval_type_name(int lval_type)
{
    switch (lval_type) {
        case LVAL_TYPE_ERROR: return "Error";
        case LVAL_TYPE_NUMBER: return "Number";
        case LVAL_TYPE_SYMBOL: return "Symbol";
        case LVAL_TYPE_STRING: return "String";
        case LVAL_TYPE_FUNC: return "Function";
        case LVAL_TYPE_SEXPR: return "S-Expression";
        case LVAL_TYPE_QEXPR: return "Q-Expression";
        case LVAL_TYPE_UNDEFINED:
        default:
            return "Undefined";
    }
}

static const char*
lval_string_escape(char c)
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

static void
lval_print_string(const struct lval* val)
{
    putchar('"');
    for (long i = 0; i < (long)strlen(val->string); i++) {
        if (strchr(STRING_ESCAPABLE_CHARS, val->string[i])) {
            printf("%s", lval_string_escape(val->string[i]));
        } else {
            putchar(val->string[i]);
        }
    }
    putchar('"');
}

void
lval_print(const struct lval* val)
{
    assert(val != NULL);

    switch (val->type) {
        case LVAL_TYPE_ERROR: printf("error: %s", val->error); break;
        case LVAL_TYPE_NUMBER: printf("%ld", val->number); break;
        case LVAL_TYPE_SYMBOL: printf("%s", val->symbol); break;
        case LVAL_TYPE_STRING: lval_print_string(val); break;
        case LVAL_TYPE_FUNC:
            if (val->builtin != NULL) {
                printf("<builtin>");
            } else {
                printf("(lambda ");
                lval_print(val->formals);
                putchar(' ');
                lval_print(val->body);
                putchar(')');
            }
            break;
        case LVAL_TYPE_SEXPR:
            putchar('(');
            for (long i = 0; i < val->cell_count; i++) {
                lval_print(val->cell[i]);
                if (i != (val->cell_count - 1)) putchar(' ');
            }
            putchar(')');
            break;
        case LVAL_TYPE_QEXPR:
            putchar('{');
            for (long i = 0; i < val->cell_count; i++) {
                lval_print(val->cell[i]);
                if (i != (val->cell_count - 1)) putchar(' ');
            }
            putchar('}');
            break;
        default: printf("undefined lisp valect"); break;
    }
}

void
lval_println(const struct lval* val)
{
    assert(val != NULL);

    lval_print(val);
    putchar('\n');
}

struct lval*
lval_list_append(struct lval* list, struct lval* val)
{
    assert(list != NULL);
    assert(val != NULL);

    list->cell_count++;
    list->cell = realloc(list->cell, sizeof(struct val*) * list->cell_count);
    list->cell[list->cell_count - 1] = val;
    return list;
}

struct lval*
lval_list_pop(struct lval* list, long i)
{
    assert(list != NULL);
    assert(i >= 0);
    assert(i < list->cell_count);

    struct lval* val = list->cell[i];
    memmove(&list->cell[i], &list->cell[i + 1], sizeof(struct lval*) * (list->cell_count - i - 1));
    list->cell_count--;
    list->cell = realloc(list->cell, sizeof(struct lval*) * list->cell_count);
    return val;
}

struct lval*
lval_list_take(struct lval* list, long i)
{
    assert(list != NULL);

    struct lval* val = lval_list_pop(list, i);
    lval_free(list);
    return val;
}

struct lval*
lval_list_join(struct lval* list, struct lval* extras)
{
    assert(list != NULL);
    assert(extras != NULL);

    while (extras->cell_count) {
        list = lval_list_append(list, lval_list_pop(extras, 0));
    }

    lval_free(extras);
    return list;
}

static struct lval*
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
    struct lval* x = NULL;
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

static struct lval*
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

    struct lval* x = lval_make_string(part);
    free(part);
    return x;
}

static struct lval*
lval_read(char* s, long* i)
{
    // skip whitespace and comments
    while (strchr(" \t\v\r\n;", s[*i]) && s[*i] != '\0') {
        if (s[*i] == ';') {
            while (s[*i] != '\n' && s[*i] != '\0') (*i)++;
        }
        (*i)++;
    }

    struct lval* x = NULL;

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

struct lval*
lval_read_expr(char* s, long* i, char end)
{
    struct lval* x = (end == '}') ? lval_make_qexpr() : lval_make_sexpr();

    while (s[*i] != end) {
        struct lval* y = lval_read(s, i);
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
