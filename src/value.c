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
    return value_is_boolean(exp) && exp->as.boolean == true;
}

bool
value_is_false(const struct value* exp)
{
    assert(exp != NULL);
    return value_is_boolean(exp) && exp->as.boolean == false;
}

bool
value_is_procedure(const struct value* exp)
{
    assert(exp != NULL);
    return value_is_builtin(exp) || value_is_lambda(exp);
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
