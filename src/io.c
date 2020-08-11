#include <assert.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "io.h"
#include "list.h"
#include "value.h"

// TODO: rewrite this using a struct that keeps track of the
//   base string and current position. could even track "start"
//   and "current" like Rob Pike's lexer.

struct value*
io_read(const char* str, long* consumed)
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
        return value_make_eof();
    }

    // quote
    if (*start == '\'') {
        const char* iter = start;
        iter++;  // consume quote char

        struct value* target = io_read(iter, consumed);
        iter += *consumed;
        *consumed = iter - str;  // consumed includes initial quote and sub read
        return list_make(2, value_make_symbol("quote"), target);
    }

    // boolean
    // TODO: character "#\"
    // TODO: vector    "#("
    // IDEA: "#u8(", "#f32(", "#f64", and other "dense" vectors
    if (*start == '#') {
        const char* iter = start;
        iter++;
        if (strchr("tf", *iter)) {
            bool boolean = *iter == 't';
            iter++;
            *consumed = iter - str;
            return value_make_boolean(boolean);
        }

        fprintf(stderr, "invalid '#' expression: expected boolean");
        exit(EXIT_FAILURE);
    }

    // number
    if (strchr(digit, *start)) {
        char* iter = NULL;
        long number = strtol(start, &iter, 10);
        *consumed = iter - str;
        return value_make_number(number);
    }

    // string
    // TODO: handle escapes
    if (*start == '"') {
        const char* iter = start;
        iter++;  // consume open quote

        while (*iter != '"' && *iter != '\0') iter++;
        if (*iter == '\0') {
            fprintf(stderr, "unterminated string");
            exit(EXIT_FAILURE);
        }

        // consume close quote
        iter++;

        *consumed = iter - str;

        // exclude quotes from the string's value
        return value_make_stringn(start + 1, iter - start - 2);
    }

    // symbol
    if (strchr(alpha, *start)) {
        const char* iter = start;
        while (strchr(alpha, *iter) && *iter != '\0') iter++;
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

        struct value* list = EMPTY_LIST;
        while (*iter != ')') {

            // read the next value
            struct value* cell = cons(io_read(iter, consumed), EMPTY_LIST);

            // if first child, replace the list ptr
            if (list == EMPTY_LIST) {
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

        return list;
    }

    fprintf(stderr, "syntax error at: TODO");
    exit(EXIT_FAILURE);
}

void
io_write(const struct value* value)
{
    if (value == EMPTY_LIST) {
        printf("'()");
        return;
    }

    switch (value->type) {
        case VALUE_BOOLEAN:
            printf("%s", value->as.boolean ? "#t" : "#f");
            break;
        case VALUE_CHARACTER:
            // TODO: how to support UTF-8 here?
            printf("%c", value->as.character);
            break;
        case VALUE_NUMBER:
            printf("%ld", value->as.number);
            break;
        case VALUE_STRING:
            // TODO: handle escapes
            printf("\"%s\"", value->as.string);
            break;
        case VALUE_SYMBOL:
            printf("%s", value->as.symbol);
            break;
        case VALUE_PAIR: {
            printf("(");
            io_write(car(value));
            printf(" . ");
            io_write(cdr(value));
            printf(")");
            break;
        }
        case VALUE_BUILTIN:
            printf("<builtin>");
            break;
        case VALUE_LAMBDA:
            printf("<lambda>");
            break;
        case VALUE_INPUT_PORT:
            printf("<input port>");
            break;
        case VALUE_OUTPUT_PORT:
            printf("<output port>");
            break;
        case VALUE_WINDOW:
            printf("<window>");
            break;
        case VALUE_EVENT: {
            switch (value->as.event->type) {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    printf("<event:%s>", "keyboard");
                    break;
                case SDL_MOUSEMOTION:
                    printf("<event:%s>", "mouse-motion");
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    printf("<event:%s>", "mouse-button");
                    break;
                case SDL_QUIT:
                    printf("<event:%s>", "quit");
                    break;
                case SDL_WINDOWEVENT:
                    printf("<event:%s>", "window");
                    break;
                default:
                    printf("<event:%s>", "undefined");
            }
            break;
        }
        case VALUE_EOF:
            printf("<EOF>");
            break;
        default:
            printf("<undefined>");
    }
}

void
io_writeln(const struct value* value)
{
    io_write(value);
    printf("\n");
}
