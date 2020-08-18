#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "value.h"

enum {
    VECTOR_INIT_CAPACITY = 8,
};

bool
value_is_empty_list(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_EMPTY_LIST;
}

bool
value_is_boolean(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_BOOLEAN;
}

bool
value_is_true(const struct value* exp)
{
    assert(exp != NULL);
    return value_is_boolean(exp) && exp->as.boolean == true;
}

bool
value_is_false(const struct value* exp)
{
    assert(exp != NULL);
    return value_is_boolean(exp) && exp->as.boolean == false;
}

bool
value_is_character(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_CHARACTER;
}

bool
value_is_number(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_NUMBER;
}

bool
value_is_string(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_STRING;
}

bool
value_is_symbol(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_SYMBOL;
}

bool
value_is_vector(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_VECTOR;
}

bool
value_is_pair(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_PAIR;
}

bool
value_is_builtin(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_BUILTIN;
}

bool
value_is_lambda(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_LAMBDA;
}

bool
value_is_procedure(const struct value* exp)
{
    assert(exp != NULL);
    return value_is_builtin(exp) || value_is_lambda(exp);
}

bool
value_is_input_port(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_INPUT_PORT;
}

bool
value_is_output_port(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_OUTPUT_PORT;
}

bool
value_is_window(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_WINDOW;
}

bool
value_is_event(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_EVENT;
}

bool
value_is_eof(const struct value* exp)
{
    assert(exp != NULL);
    return exp->type == VALUE_EOF;
}

struct value*
value_make_empty_list(void)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_EMPTY_LIST;
    return value;
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
value_make_character(int character)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_CHARACTER;
    value->as.character = character;
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
value_make_vector(void)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_VECTOR;
    value->as.vector.count = 0;
    value->as.vector.capacity = VECTOR_INIT_CAPACITY;
    value->as.vector.array = malloc(VECTOR_INIT_CAPACITY * sizeof(struct value*));
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
value_make_input_port(FILE* port)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_INPUT_PORT;
    value->as.port = port;
    return value;
}

struct value*
value_make_output_port(FILE* port)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_OUTPUT_PORT;
    value->as.port = port;
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

struct value*
value_make_eof(void)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_EOF;
    return value;
}

// TODO: move this into GC routine
static void
value_free(struct value* value)
{
    switch (value->type) {
        case VALUE_EMPTY_LIST:
            break;
        case VALUE_BOOLEAN:
            break;
        case VALUE_CHARACTER:
            break;
        case VALUE_NUMBER:
            break;
        case VALUE_STRING:
            free(value->as.string);
            break;
        case VALUE_SYMBOL:
            free(value->as.symbol);
            break;
        case VALUE_VECTOR:
            for (long i = 0; i < value->as.vector.count; i++) {
                value_free(value->as.vector.array[i]);
            }
            free(value->as.vector.array);
        case VALUE_PAIR:
            break;
        case VALUE_BUILTIN:
            break;
        case VALUE_LAMBDA:
            value_free(value->as.lambda.params);
            value_free(value->as.lambda.body);
            value_free(value->as.lambda.env);
            break;
        case VALUE_INPUT_PORT:
        case VALUE_OUTPUT_PORT:
            fclose(value->as.port);
            break;
        case VALUE_WINDOW:
            SDL_DestroyRenderer(value->as.window.renderer);
            SDL_DestroyWindow(value->as.window.window);
            break;
        case VALUE_EVENT:
            free(value->as.event);
            break;
        case VALUE_EOF:
            break;
    }

    free(value);
}

void
value_print(FILE* fp, const struct value* value)
{
    switch (value->type) {
        case VALUE_EMPTY_LIST:
            fprintf(fp, "'()");
            break;
        case VALUE_BOOLEAN:
            fprintf(fp, "#%c", value->as.boolean ? 't' : 'f');
            break;
        case VALUE_CHARACTER:
            // TODO: how to support UTF-8 here?
            if (value->as.character == ' ') {
                fprintf(fp, "#\\space");
            } else if (value->as.character == '\n') {
                fprintf(fp, "#\\newline");
            } else {
                fprintf(fp, "#\\%c", value->as.character);
            }
            break;
        case VALUE_NUMBER:
            fprintf(fp, "%ld", value->as.number);
            break;
        case VALUE_STRING:
            // TODO: handle escapes
            fprintf(fp, "\"%s\"", value->as.string);
            break;
        case VALUE_SYMBOL:
            fprintf(fp, "%s", value->as.symbol);
            break;
        case VALUE_VECTOR:
            fprintf(fp, "#(");
            for (long i = 0; i < value->as.vector.count; i++) {
                value_print(fp, value->as.vector.array[i]);
                if (i < value->as.vector.count - 1) fprintf(fp, " ");
            }
            fprintf(fp, ")");
            break;
        case VALUE_PAIR: {
            fprintf(fp, "(");
            value_print(fp, value->as.pair.car);
            fprintf(fp, " . ");
            value_print(fp, value->as.pair.cdr);
            fprintf(fp, ")");
            break;
        }
        case VALUE_BUILTIN:
            fprintf(fp, "<builtin>");
            break;
        case VALUE_LAMBDA:
            fprintf(fp, "<lambda>");
            break;
        case VALUE_INPUT_PORT:
            fprintf(fp, "<input port>");
            break;
        case VALUE_OUTPUT_PORT:
            fprintf(fp, "<output port>");
            break;
        case VALUE_WINDOW:
            fprintf(fp, "<window>");
            break;
        case VALUE_EVENT: {
            switch (value->as.event->type) {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    fprintf(fp, "<event:%s>", "keyboard");
                    break;
                case SDL_MOUSEMOTION:
                    fprintf(fp, "<event:%s>", "mouse-motion");
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    fprintf(fp, "<event:%s>", "mouse-button");
                    break;
                case SDL_QUIT:
                    fprintf(fp, "<event:%s>", "quit");
                    break;
                case SDL_WINDOWEVENT:
                    fprintf(fp, "<event:%s>", "window");
                    break;
                default:
                    fprintf(fp, "<event:%s>", "undefined");
            }
            break;
        }
        case VALUE_EOF:
            fprintf(fp, "<EOF>");
            break;
        default:
            fprintf(fp, "<undefined>");
    }
}

void
value_println(FILE* fp, const struct value* value)
{
    value_print(fp, value);
    printf("\n");
}

const char*
value_type_name(int type)
{
    switch (type) {
        case VALUE_EMPTY_LIST: return "Empty List";
        case VALUE_BOOLEAN: return "Boolean";
        case VALUE_CHARACTER: return "Character";
        case VALUE_NUMBER: return "Number";
        case VALUE_STRING: return "String";
        case VALUE_SYMBOL: return "Symbol";
        case VALUE_VECTOR: return "Vector";
        case VALUE_PAIR: return "Pair";
        case VALUE_BUILTIN: return "Builtin";
        case VALUE_LAMBDA: return "Lambda";
        case VALUE_INPUT_PORT: return "Input Port";
        case VALUE_OUTPUT_PORT: return "Output Port";
        case VALUE_WINDOW: return "Window";
        case VALUE_EVENT: return "Event";
        case VALUE_EOF: return "EOF";
        default: return "Undefined";
    }
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
    if (a->type != b->type) return false;

    switch (a->type) {
        case VALUE_EMPTY_LIST:
            // all instances of the empty list are the same
            return true;
        case VALUE_BOOLEAN:
            return a->as.boolean == b->as.boolean;
        case VALUE_CHARACTER:
            return a->as.character == b->as.character;
        case VALUE_NUMBER:
            return a->as.number == b->as.number;
        case VALUE_STRING:
            return strcmp(a->as.string, b->as.string) == 0;
        case VALUE_SYMBOL:
            return strcmp(a->as.symbol, b->as.symbol) == 0;
        case VALUE_VECTOR:
            if (a->as.vector.count != b->as.vector.count) return false;
            for (long i = 0; i < a->as.vector.count; i++) {
                if (!value_is_equal(a->as.vector.array[i], b->as.vector.array[i])) {
                    return false;
                }
            }
            return true;
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
        case VALUE_INPUT_PORT:
        case VALUE_OUTPUT_PORT:
            // direct pointer compare
            return a->as.port == b->as.port;
        case VALUE_WINDOW:
            // direct pointer compare
            return a->as.window.window == b->as.window.window &&
                   a->as.window.renderer == b->as.window.renderer;
        case VALUE_EVENT:
            // two events are never equal?
            return false;
        case VALUE_EOF:
            // all instances of EOF are the same
            return true;
    }

    return false;
}
