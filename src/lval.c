#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mpc.h>

#include "lenv.h"
#include "lval.h"

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

static void
lval_print_string(const struct lval* val)
{
    char* escaped = malloc(strlen(val->string) + 1);
    strcpy(escaped, val->string);

    escaped = mpcf_escape(escaped);
    printf("\"%s\"", escaped);
    free(escaped);
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
lval_read_number(mpc_ast_t* ast)
{
    errno = 0;
    long x = strtol(ast->contents, NULL, 10);
    return errno != ERANGE ? lval_make_number(x) : lval_make_error("invalid number");
}

static struct lval*
lval_read_string(mpc_ast_t* ast)
{
    // cut off final quote char
    ast->contents[strlen(ast->contents) - 1] = '\0';

    // copy the string except first quote char
    char* unescaped = malloc(strlen(ast->contents + 1) + 1);
    strcpy(unescaped, ast->contents + 1);

    // unescape and make the lval
    unescaped = mpcf_unescape(unescaped);
    struct lval* string = lval_make_string(unescaped);

    // free the unescaped string copy and return
    free(unescaped);
    return string;
}

struct lval*
lval_read(mpc_ast_t* ast)
{   // TODO: don't treat root exprs as sexprs... its weird
    // if number or symbol, convert to that type
    if (strstr(ast->tag, "number")) return lval_read_number(ast);
    if (strstr(ast->tag, "symbol")) return lval_make_symbol(ast->contents);
    if (strstr(ast->tag, "string")) return lval_read_string(ast);

    // if root or sexpr then create an empty list
    struct lval* list = NULL;
    if (strcmp(ast->tag, ">") == 0) list = lval_make_sexpr();
    if (strstr(ast->tag, "sexpr")) list = lval_make_sexpr();
    if (strstr(ast->tag, "qexpr")) list = lval_make_qexpr();

    // fill the list with any valid sub-expressions
    for (long i = 0; i < ast->children_num; i++) {
        if (strcmp(ast->children[i]->contents, "(") == 0) continue;
        if (strcmp(ast->children[i]->contents, ")") == 0) continue;
        if (strcmp(ast->children[i]->contents, "{") == 0) continue;
        if (strcmp(ast->children[i]->contents, "}") == 0) continue;
        if (strcmp(ast->children[i]->tag, "regex") == 0) continue;
        if (strcmp(ast->children[i]->tag, "comment") == 0) continue;
        list = lval_list_append(list, lval_read(ast->children[i]));
    }

    return list;
}
