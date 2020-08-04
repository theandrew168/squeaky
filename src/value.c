#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "value.h"

bool
value_is_true(const struct value* exp)
{
    assert(exp != NULL);

    return value_is_boolean(exp) &&
           exp->as.boolean == true;
}

bool
value_is_false(const struct value* exp)
{
    assert(exp != NULL);

    return value_is_boolean(exp) &&
           exp->as.boolean == false;
}

bool
value_is_procedure(const struct value* exp)
{
    assert(exp != NULL);
    return value_is_builtin(exp) || value_is_lambda(exp);
}

struct value*
value_make_boolean(bool boolean)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_BOOLEAN;
    value->ref_count = 1;
    value->as.boolean = boolean;
    return value;
}

struct value*
value_make_number(long number)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_NUMBER;
    value->ref_count = 1;
    value->as.number = number;
    return value;
}

struct value*
value_make_string(const char* string)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_STRING;
    value->ref_count = 1;
    value->as.string = malloc(strlen(string) + 1);
    strcpy(value->as.string, string);
    return value;
}

struct value*
value_make_stringn(const char* string, long length)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_STRING;
    value->ref_count = 1;
    value->as.string = malloc(length + 1);
    snprintf(value->as.string, length + 1, "%s", string);
    return value;
}

struct value*
value_make_symbol(const char* symbol)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_SYMBOL;
    value->ref_count = 1;
    value->as.symbol = malloc(strlen(symbol) + 1);
    strcpy(value->as.symbol, symbol);
    return value;
}

struct value*
value_make_symboln(const char* symbol, long length)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_SYMBOL;
    value->ref_count = 1;
    value->as.symbol = malloc(length + 1);
    snprintf(value->as.symbol, length + 1, "%s", symbol);
    return value;
}

struct value*
value_make_pair(struct value* car, struct value* cdr)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_PAIR;
    value->ref_count = 1;
    value->as.pair.car = car;
    value->as.pair.cdr = cdr;
    return value;
}

struct value*
value_make_builtin(builtin_func builtin)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_BUILTIN;
    value->ref_count = 1;
    value->as.builtin = builtin;
    return value;
}

struct value*
value_make_lambda(struct value* params, struct value* body, struct value* env)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_LAMBDA;
    value->ref_count = 1;
    value->as.lambda.params = params;
    value->as.lambda.body = body;
    value->as.lambda.env = env;
    return value;
}

struct value*
value_make_window(const char* title, long width, long height)
{
    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        0);
    if (window == NULL) {
        fprintf(stderr, "failed to create SDL2 window: %s", SDL_GetError());
        return value_make_error("failed to create SDL2 window");
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "failed to create SDL2 renderer: %s", SDL_GetError());
        return value_make_error("failed to create SDL2 renderer");
    }

    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_WINDOW;
    value->ref_count = 1;
    value->as.window.window = window;
    value->as.window.renderer = renderer;
    return value;
}

struct value*
value_make_error(const char* error)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_ERROR;
    value->ref_count = 1;
    value->as.error = malloc(strlen(error) + 1);
    strcpy(value->as.error, error);
    return value;
}

void
value_free(struct value* value)
{
    if (value == NULL) return;

    switch (value->type) {
        case VALUE_BOOLEAN:
            break;
        case VALUE_NUMBER:
            break;
        case VALUE_STRING:
            free(value->as.string);
            break;
        case VALUE_SYMBOL:
            free(value->as.symbol);
            break;
        case VALUE_PAIR:
            value_free(value->as.pair.car);
            value_free(value->as.pair.cdr);
            break;
        case VALUE_BUILTIN:
            break;
        case VALUE_LAMBDA:
            value_free(value->as.lambda.params);
            value_free(value->as.lambda.body);
            value_free(value->as.lambda.env);
            break;
        case VALUE_WINDOW:
            SDL_DestroyRenderer(value->as.window.renderer);
            SDL_DestroyWindow(value->as.window.window);
            break;
        case VALUE_ERROR:
            free(value->as.error);
            break;
    }

    free(value);
}

bool
value_equal(const struct value* a, const struct value* b)
{
    assert(a != NULL);
    assert(b != NULL);

    if (a->type != b->type) return false;

    switch (a->type) {
        case VALUE_BOOLEAN:
            return a->as.boolean == b->as.boolean;
        case VALUE_NUMBER:
            return a->as.number == b->as.number;
        case VALUE_STRING:
            return strcmp(a->as.string, b->as.string) == 0;
        case VALUE_SYMBOL:
            return strcmp(a->as.symbol, b->as.symbol) == 0;
        case VALUE_PAIR:
            return value_equal(a->as.pair.car, b->as.pair.car) &&
                   value_equal(a->as.pair.cdr, b->as.pair.cdr);
        case VALUE_BUILTIN:
            // direct pointer compare
            return a->as.builtin == b->as.builtin;
        case VALUE_LAMBDA:
            return value_equal(a->as.lambda.params, b->as.lambda.params) &&
                   value_equal(a->as.lambda.body, b->as.lambda.body) &&
                   value_equal(a->as.lambda.env, b->as.lambda.env);
        case VALUE_WINDOW:
            // direct pointer compare
            return a->as.window.window == b->as.window.window &&
                   a->as.window.renderer == b->as.window.renderer;
        case VALUE_ERROR:
            return strcmp(a->as.error, b->as.error) == 0;
    }

    return false;
}

struct value*
value_read(const char* str, long* consumed)
{
    const char space[] = " \f\n\r\t\v;";
    const char digit[] = "0123456789";
    const char alpha[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "+-!$%&*/:<=>?^_~";

    // needed to capture how many chars this value consumes
    const char* start = str;

    // whitespace / comments
    while (strchr(space, *start) && *start != '\0') {
        if (*start == ';') {
            while (*start != '\n' && *start != '\0') start++;
        }
        start++;
    }

    // EOF
    if (*start == '\0') {
        *consumed = start - str;
        return value_make_pair(NULL, NULL);
        return value_make_error("unexpected EOF");
    }

    // quote
    if (*start == '\'') {
        const char* iter = start;
        iter++;  // consume quote char

        struct value* target = value_read(iter, consumed);
        iter += *consumed;
        *consumed = iter - str;  // consumed includes initial quote and sub read
        return list_make(value_make_symbol("quote"), target, NULL);
    }

    // boolean
    // TODO: character "#\"
    // TODO: vector    "#("
    if (*start == '#') {
        const char* iter = start;
        iter++;
        if (strchr("tf", *iter)) {
            bool boolean = *iter == 't';
            iter++;
            *consumed = iter - str;
            return value_make_boolean(boolean);
        }

        return value_make_error("invalid expression");
    }

    // number
    if (strchr(digit, *start)) {
        char* iter = NULL;
        long number = strtol(start, &iter, 10);
        *consumed = iter - str;
        return value_make_number(number);
    }

    // string
    if (*start == '"') {
        const char* iter = start;
        iter++;  // consume open quote

        while (*iter != '"' && *iter != '\0') iter++;
        if (*iter == '\0') return value_make_error("unterminated string");

        // consume close quote
        iter++;

        *consumed = iter - str;

        // exclude quotes from the string's value
        return value_make_stringn(start + 1, iter - start - 2);
    }

    // symbol
    if (strchr(alpha, *start)) {
        const char* iter = start;
        while (strchr(alpha, *iter)) iter++;
        *consumed = iter - str;
        return value_make_symboln(start, iter - start);
    }

    // pair / list / s-expression
    // TODO: this case is ugly, need to read hella book
    if (*start == '(') {
        const char* iter = start;
        iter++;  // skip open paren

        // whitespace / comments (again)
        while (strchr(space, *iter) && *iter != '\0') {
            if (*iter == ';') {
                while (*iter != '\n' && *iter != '\0') iter++;
            }
            iter++;
        }

        struct value* list = NULL;
        while (*iter != ')') {

            // read the next value
            struct value* cell = cons(value_read(iter, consumed), NULL);

            // if first child, replace the list ptr
            if (list == NULL) {
                list = cell;
            } else {  // else cons like normal
                struct value* last = list;
                while (cdr(last) != NULL) last = cdr(last);
                last->as.pair.cdr = cell;
            }

            // advance to end of read value
            iter += *consumed;

            // whitespace / comments (AGAIN)
            while (strchr(space, *iter) && *iter != '\0') {
                if (*iter == ';') {
                    while (*iter != '\n' && *iter != '\0') iter++;
                }
                iter++;
            }
        }

        iter++;  // move past the closing paren
        *consumed = iter - str;

        // special case if list never got built
        if (list == NULL) return cons(NULL, NULL);

        return list;
    }

    return value_make_error("invalid expression");
}

// TODO: condense pair printing
// but pair would need to be simplified once all
//   the cars / cdrs are settled
void
value_write(const struct value* value)
{
    assert(value != NULL);

    switch (value->type) {
        case VALUE_BOOLEAN:
            printf("%s", value->as.boolean ? "#t" : "#f");
            break;
        case VALUE_NUMBER:
            printf("%ld", value->as.number);
            break;
        case VALUE_STRING:
            printf("\"%s\"", value->as.string);
            break;
        case VALUE_SYMBOL:
            printf("%s", value->as.symbol);
            break;
        case VALUE_PAIR:
            if (value->as.pair.car == NULL && value->as.pair.cdr == NULL) {
                printf("ok");
                break;
            }
            printf("(");
            if (value->as.pair.car == NULL) printf("'()");
            else value_write(value->as.pair.car);
            printf(" . ");
            if (value->as.pair.cdr == NULL) printf("'()");
            else value_write(value->as.pair.cdr);
            printf(")");
            break;
        case VALUE_BUILTIN:
            printf("<builtin>");
            break;
        case VALUE_LAMBDA:
            printf("<lambda>");
            break;
        case VALUE_WINDOW:
            printf("<window>");
            break;
        case VALUE_ERROR:
            printf("error: %s", value->as.error);
            break;
        default:
            printf("<undefined>");
    }
}

struct value*
list_make(struct value* value, ...)
{
    va_list args;
    va_start(args, value);

    struct value* head = cons(value, NULL);
    struct value* tail = head;

    for (;;) {
        struct value* v = va_arg(args, struct value*);
        if (v == NULL) break;

        tail->as.pair.cdr = cons(v, NULL);
        tail = cdr(tail);
    }

    va_end(args);
    return head;
}

long
list_length(const struct value* list)
{
    long count = 0;

    const struct value* iter = list;
    while (iter != NULL) {
        count++;
        iter = cdr(iter);
    }

    return count;
}

bool
list_is_null(const struct value* list)
{
    assert(list != NULL);

    if (!value_is_pair(list)) return false;
    return list->as.pair.car == NULL && list->as.pair.cdr == NULL;
}

struct value*
list_car(const struct value* list)
{
    assert(list != NULL);

    if (list_is_null(list)) return value_make_error("the primitive 'car' is defined only for non-empty lists");
    return list->as.pair.car;
}

struct value*
list_cdr(const struct value* list)
{
    assert(list != NULL);

    if (list_is_null(list)) return value_make_error("the primitive 'cdr' is defined only for non-empty lists");
    return list->as.pair.cdr;
}
