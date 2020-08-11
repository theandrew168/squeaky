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
value_is_boolean(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_BOOLEAN;
}

bool
value_is_true(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return value_is_boolean(exp) &&
           exp->as.boolean == true;
}

bool
value_is_false(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return value_is_boolean(exp) &&
           exp->as.boolean == false;
}

bool
value_is_number(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_NUMBER;
}

bool
value_is_string(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_STRING;
}

bool
value_is_symbol(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_SYMBOL;
}

bool
value_is_pair(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_PAIR;
}

bool
value_is_builtin(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_BUILTIN;
}

bool
value_is_lambda(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_LAMBDA;
}

bool
value_is_procedure(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return value_is_builtin(exp) || value_is_lambda(exp);
}

bool
value_is_input_port(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_INPUT_PORT;
}

bool
value_is_output_port(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_OUTPUT_PORT;
}

bool
value_is_window(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_WINDOW;
}

bool
value_is_event(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_EVENT;
}

bool
value_is_eof(const struct value* exp)
{
    if (exp == EMPTY_LIST) return false;
    return exp->type == VALUE_EOF;
}

struct value*
value_make_boolean(bool boolean)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_BOOLEAN;
    value->as.boolean = boolean;
    return value;
}

struct value*
value_make_number(long number)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_NUMBER;
    value->as.number = number;
    return value;
}

struct value*
value_make_string(const char* string)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_STRING;
    value->as.string = malloc(strlen(string) + 1);
    strcpy(value->as.string, string);
    return value;
}

struct value*
value_make_stringn(const char* string, long length)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_STRING;
    value->as.string = malloc(length + 1);
    snprintf(value->as.string, length + 1, "%s", string);
    return value;
}

struct value*
value_make_symbol(const char* symbol)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_SYMBOL;
    value->as.symbol = malloc(strlen(symbol) + 1);
    strcpy(value->as.symbol, symbol);
    return value;
}

struct value*
value_make_symboln(const char* symbol, long length)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_SYMBOL;
    value->as.symbol = malloc(length + 1);
    snprintf(value->as.symbol, length + 1, "%s", symbol);
    return value;
}

struct value*
value_make_pair(struct value* car, struct value* cdr)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_PAIR;
    value->as.pair.car = car;
    value->as.pair.cdr = cdr;
    return value;
}

struct value*
value_make_builtin(builtin_func builtin)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_BUILTIN;
    value->as.builtin = builtin;
    return value;
}

struct value*
value_make_lambda(struct value* params, struct value* body, struct value* env)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_LAMBDA;
    value->as.lambda.params = params;
    value->as.lambda.body = body;
    value->as.lambda.env = env;
    return value;
}

struct value*
value_make_eof(void)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_EOF;
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
        fprintf(stderr, "failed to create SDL2 window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "failed to create SDL2 renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_WINDOW;
    value->as.window.window = window;
    value->as.window.renderer = renderer;
    return value;
}

struct value*
value_make_event(SDL_Event* event)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_EVENT;
    value->as.event = event;
    return value;
}

static void
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
            break;
        case VALUE_BUILTIN:
            break;
        case VALUE_LAMBDA:
            value_free(value->as.lambda.params);
            value_free(value->as.lambda.body);
            value_free(value->as.lambda.env);
            break;
        case VALUE_EOF:
            break;
        case VALUE_WINDOW:
            SDL_DestroyRenderer(value->as.window.renderer);
            SDL_DestroyWindow(value->as.window.window);
            break;
        case VALUE_EVENT:
            free(value->as.event);
            break;
    }

    free(value);
}

bool
value_is_eq(const struct value* a, const struct value* b)
{
    // TODO: how is this more specific than eqv?
    return value_is_eqv(a, b);
}

bool
value_is_eqv(const struct value* a, const struct value* b)
{
    // TODO: this shouldn't do a deep compare!
    return value_is_equal(a, b);
}

bool
value_is_equal(const struct value* a, const struct value* b)
{
    if (a == EMPTY_LIST && b == EMPTY_LIST) return true;
    if (a == EMPTY_LIST || b == EMPTY_LIST) return false;
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
            return value_is_equal(a->as.pair.car, b->as.pair.car) &&
                   value_is_equal(a->as.pair.cdr, b->as.pair.cdr);
        case VALUE_BUILTIN:
            // direct pointer compare
            return a->as.builtin == b->as.builtin;
        case VALUE_LAMBDA:
            return value_is_equal(a->as.lambda.params, b->as.lambda.params) &&
                   value_is_equal(a->as.lambda.body, b->as.lambda.body) &&
                   value_is_equal(a->as.lambda.env, b->as.lambda.env);
        case VALUE_EOF:
            // all instances of EOF are semantically identical
            return true;
        case VALUE_WINDOW:
            // direct pointer compare
            return a->as.window.window == b->as.window.window &&
                   a->as.window.renderer == b->as.window.renderer;
        case VALUE_EVENT:
            // two events are never equal?
            return false;
    }

    return false;
}
