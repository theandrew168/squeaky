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
value_make_event(SDL_Event event)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_EVENT;
    value->ref_count = 1;
    value->as.event = event;
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
        case VALUE_EVENT:
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
        case VALUE_EVENT:
            // two events are never equal?
            return false;
        case VALUE_ERROR:
            return strcmp(a->as.error, b->as.error) == 0;
    }

    return false;
}
