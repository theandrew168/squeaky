#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "builtin.h"
#include "list.h"
#include "reader.h"
#include "value.h"

#define ASSERTF(cond, fmt, ...)           \
  if (!(cond)) {                          \
    fprintf(stderr, fmt, ##__VA_ARGS__);  \
    exit(EXIT_FAILURE);                   \
  }

#define ASSERT_ARITY(func, args, count)                                   \
  ASSERTF(list_length(args) == count,                                     \
    "function '%s' passed incorrect number of args: want %d, got %ld\n",  \
    func, count, list_length(args))

#define ASSERT_ARITY_OR(func, args, a, b)                                       \
  ASSERTF(list_length(args) == (a) || list_length(args) == (b),                 \
    "function '%s' passed incorrect number of args: want %d or %d, got %ld\n",  \
    func, a, b, list_length(args))

#define ASSERT_ARITY_LTE(func, args, count)                            \
  ASSERTF(list_length(args) <= count,                                  \
    "function '%s' passed too many args: want at most %d, got %ld\n",  \
    func, count, list_length(args))

#define ASSERT_ARITY_GTE(func, args, count)                            \
  ASSERTF(list_length(args) >= count,                                  \
    "function '%s' passed too few args: want at least %d, got %ld\n",  \
    func, count, list_length(args))

#define ASSERT_TYPE(func, args, index, want)                              \
  ASSERTF(list_nth(args, index)->type == want,                            \
    "function '%s' passed incorrect type for arg %i: want %s, got %s\n",  \
    func, index,                                                          \
    value_type_name(want),                                                \
    value_type_name(list_nth(args, index)->type))

#define ASSERT_TYPE_ALL(func, args, want)        \
  for (int i = 0; i < list_length(args); i++) {  \
    ASSERT_TYPE(func, args, i, want)             \
  }

struct value*
builtin_is_eq(struct value* args)
{
    ASSERT_ARITY("eq?", args, 2);

    struct value* a = car(args);
    struct value* b = cadr(args);

    return value_is_eq(a, b) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_eqv(struct value* args)
{
    ASSERT_ARITY("eqv?", args, 2);

    struct value* a = car(args);
    struct value* b = cadr(args);

    return value_is_eqv(a, b) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_equal(struct value* args)
{
    ASSERT_ARITY("equal?", args, 2);

    struct value* a = car(args);
    struct value* b = cadr(args);

    return value_is_equal(a, b) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_number(struct value* args)
{
    ASSERT_ARITY("number?", args, 1);

    return value_is_number(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_equal(struct value* args)
{
    ASSERT_ARITY_GTE("=", args, 2);
    ASSERT_TYPE_ALL("=", args, VALUE_NUMBER);

    struct value* item = car(args);
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        if (!value_is_equal(item, car(args))) return value_make_boolean(false);
        item = car(args);
    }

    return value_make_boolean(true);
}

struct value*
builtin_less(struct value* args)
{
    ASSERT_ARITY_GTE("<", args, 2);
    ASSERT_TYPE_ALL("<", args, VALUE_NUMBER);

    struct value* item = car(args);
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        if (!(item->as.number < car(args)->as.number)) return value_make_boolean(false);
        item = car(args);
    }

    return value_make_boolean(true);
}

struct value*
builtin_greater(struct value* args)
{
    ASSERT_ARITY_GTE(">", args, 2);
    ASSERT_TYPE_ALL(">", args, VALUE_NUMBER);

    struct value* item = car(args);
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        if (!(item->as.number > car(args)->as.number)) return value_make_boolean(false);
        item = car(args);
    }

    return value_make_boolean(true);
}

struct value*
builtin_less_equal(struct value* args)
{
    ASSERT_ARITY_GTE("<=", args, 2);
    ASSERT_TYPE_ALL("<=", args, VALUE_NUMBER);

    struct value* item = car(args);
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        if (!(item->as.number <= car(args)->as.number)) return value_make_boolean(false);
        item = car(args);
    }

    return value_make_boolean(true);
}

struct value*
builtin_greater_equal(struct value* args)
{
    ASSERT_ARITY_GTE(">=", args, 2);
    ASSERT_TYPE_ALL(">=", args, VALUE_NUMBER);

    struct value* item = car(args);
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        if (!(item->as.number >= car(args)->as.number)) return value_make_boolean(false);
        item = car(args);
    }

    return value_make_boolean(true);
}

struct value*
builtin_add(struct value* args)
{
    ASSERT_ARITY_GTE("+", args, 2);
    ASSERT_TYPE_ALL("+", args, VALUE_NUMBER);

    long res = 0;
    for (; args != NULL; args = cdr(args)) {
        res += car(args)->as.number;
    }

    return value_make_number(res);
}

struct value*
builtin_mul(struct value* args)
{
    ASSERT_ARITY_GTE("*", args, 2);
    ASSERT_TYPE_ALL("*", args, VALUE_NUMBER);

    long res = 1;
    for (; args != NULL; args = cdr(args)) {
        res *= car(args)->as.number;
    }

    return value_make_number(res);
}

struct value*
builtin_sub(struct value* args)
{
    ASSERT_ARITY_GTE("-", args, 2);
    ASSERT_TYPE_ALL("-", args, VALUE_NUMBER);

    long res = car(args)->as.number;
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        res -= car(args)->as.number;
    }

    return value_make_number(res);
}

struct value*
builtin_div(struct value* args)
{
    ASSERT_ARITY_GTE("/", args, 2);
    ASSERT_TYPE_ALL("/", args, VALUE_NUMBER);

    long res = car(args)->as.number;
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        if (car(args)->as.number == 0) {
            fprintf(stderr, "error: divide by zero\n");
            exit(EXIT_FAILURE);
        }

        res /= car(args)->as.number;
    }

    return value_make_number(res);
}

struct value*
builtin_is_boolean(struct value* args)
{
    ASSERT_ARITY("boolean?", args, 1);

    return value_is_boolean(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_pair(struct value* args)
{
    ASSERT_ARITY("pair?", args, 1);

    return value_is_pair(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_cons(struct value* args)
{
    ASSERT_ARITY("cons", args, 2);

    struct value* a = car(args);
    struct value* b = cadr(args);
    return cons(a, b);
}

struct value*
builtin_car(struct value* args)
{
    ASSERT_ARITY("car", args, 1);
    ASSERT_TYPE("car", args, 0, VALUE_PAIR);

    struct value* pair = car(args);
    return car(pair);
}

struct value*
builtin_cdr(struct value* args)
{
    ASSERT_ARITY("cdr", args, 1);
    ASSERT_TYPE("cdr", args, 0, VALUE_PAIR);

    struct value* pair = car(args);
    return cdr(pair);
}

struct value*
builtin_set_car(struct value* args)
{
    ASSERT_ARITY("set-car!", args, 2);
    ASSERT_TYPE("set-car!", args, 0, VALUE_PAIR);

    struct value* pair = car(args);
    struct value* val = cadr(args);
    pair->as.pair.car = val;
    return EMPTY_LIST;
}

struct value*
builtin_set_cdr(struct value* args)
{
    ASSERT_ARITY("set-cdr!", args, 2);
    ASSERT_TYPE("set-cdr!", args, 0, VALUE_PAIR);

    struct value* pair = car(args);
    struct value* val = cadr(args);
    pair->as.pair.cdr = val;
    return EMPTY_LIST;
}

struct value*
builtin_is_null(struct value* args)
{
    ASSERT_ARITY("null?", args, 1);

    return car(args) == EMPTY_LIST ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_symbol(struct value* args)
{
    ASSERT_ARITY("symbol?", args, 1);

    return value_is_symbol(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_string(struct value* args)
{
    ASSERT_ARITY("string?", args, 1);

    return value_is_string(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_procedure(struct value* args)
{
    ASSERT_ARITY("procedure?", args, 1);

    return value_is_procedure(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_input_port(struct value* args)
{
    ASSERT_ARITY("input-port?", args, 1);

    return value_is_input_port(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_output_port(struct value* args)
{
    ASSERT_ARITY("output-port?", args, 1);

    return value_is_output_port(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

// TODO: is there a better way to handle this?
struct value*
builtin_current_input_port(struct value* args)
{
    ASSERT_ARITY("current-input-port", args, 0);

    return value_make_input_port(stdin);
}

// TODO: is there a better way to handle this?
struct value*
builtin_current_output_port(struct value* args)
{
    ASSERT_ARITY("current-output-port", args, 0);

    return value_make_output_port(stdout);
}

struct value*
builtin_open_input_file(struct value* args)
{
    ASSERT_ARITY("open-input-file", args, 1);
    ASSERT_TYPE("open-input-file", args, 0, VALUE_STRING);

    struct value* path = car(args);

    FILE* port = fopen(path->as.string, "rb");
    if (port == NULL) {
        fprintf(stderr, "failed to open input file: %s\n", path->as.string);
        perror("reason");
        exit(EXIT_FAILURE);
    }

    return value_make_input_port(port);
}

struct value*
builtin_open_output_file(struct value* args)
{
    ASSERT_ARITY("open-output-file", args, 1);
    ASSERT_TYPE("open-output-file", args, 0, VALUE_STRING);

    struct value* path = car(args);

    FILE* port = fopen(path->as.string, "wb");
    if (port == NULL) {
        fprintf(stderr, "failed to open output file: %s\n", path->as.string);
        perror("reason");
        exit(EXIT_FAILURE);
    }

    return value_make_output_port(port);
}

struct value*
builtin_close_input_port(struct value* args)
{
    ASSERT_ARITY("close-input-port", args, 1);
    ASSERT_TYPE("close-input-port", args, 0, VALUE_INPUT_PORT);

    struct value* port = car(args);
    fclose(port->as.port);

    return EMPTY_LIST;
}

struct value*
builtin_close_output_port(struct value* args)
{
    ASSERT_ARITY("close-output-port", args, 1);
    ASSERT_TYPE("close-output-port", args, 0, VALUE_OUTPUT_PORT);

    struct value* port = car(args);
    fclose(port->as.port);

    return EMPTY_LIST;
}

struct value*
builtin_read(struct value* args)
{
    ASSERT_ARITY_OR("read", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("read", args, 0, VALUE_INPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = car(args);
    } else {
        port = value_make_input_port(stdin);
    }

    FILE* fp = port->as.port;
    return reader_read(fp);
}

struct value*
builtin_read_char(struct value* args)
{
    ASSERT_ARITY_OR("read-char", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("read-char", args, 0, VALUE_INPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = car(args);
    } else {
        port = value_make_input_port(stdin);
    }

    FILE* fp = port->as.port;

    int c = fgetc(fp);
    if (ferror(fp)) {
        perror("error reading from port");
        exit(EXIT_FAILURE);
    }

    if (c == EOF && feof(fp)) {
        return value_make_eof();
    }

    return value_make_character(c);
}

struct value*
builtin_peek_char(struct value* args)
{
    ASSERT_ARITY_OR("peek-char", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("peek-char", args, 0, VALUE_INPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = car(args);
    } else {
        port = value_make_input_port(stdin);
    }

    FILE* fp = port->as.port;

    int c = fgetc(fp);
    if (ferror(fp)) {
        perror("error reading from port");
        exit(EXIT_FAILURE);
    }

    if (c == EOF && feof(fp)) {
        return value_make_eof();
    }

    ungetc(c, fp);

    return value_make_character(c);
}

struct value*
builtin_is_eof_object(struct value* args)
{
    ASSERT_ARITY("eof-object?", args, 1);

    return value_is_eof(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_char_ready(struct value* args)
{
    ASSERT_ARITY_OR("char-ready?", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("char-ready?", args, 0, VALUE_INPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = car(args);
    } else {
        port = value_make_input_port(stdin);
    }

    FILE* fp = port->as.port;

    int c = fgetc(fp);
    if (ferror(fp)) {
        perror("error reading from port");
        exit(EXIT_FAILURE);
    }

    if (c == EOF && feof(fp)) {
        return value_make_boolean(true);
    }

    ungetc(c, fp);

    return value_make_boolean(true);
}

struct value*
builtin_write(struct value* args)
{
    ASSERT_ARITY_OR("write", args, 1, 2);

    long arity = list_length(args);
    if (arity == 2) {
        ASSERT_TYPE("write", args, 1, VALUE_OUTPUT_PORT);
    }

    struct value* obj = car(args);

    struct value* port = NULL;
    if (arity == 2) {
        port = cadr(args);
    } else {
        port = value_make_output_port(stdout);
    }

    value_print(port->as.port, obj);
    return EMPTY_LIST;
}

struct value*
builtin_display(struct value* args)
{
    ASSERT_ARITY_OR("display", args, 1, 2);

    long arity = list_length(args);
    if (arity == 2) {
        ASSERT_TYPE("display", args, 1, VALUE_OUTPUT_PORT);
    }

    struct value* obj = car(args);

    struct value* port = NULL;
    if (arity == 2) {
        port = cadr(args);
    } else {
        port = value_make_output_port(stdout);
    }

    value_print(port->as.port, obj);
    return EMPTY_LIST;
}

struct value*
builtin_newline(struct value* args)
{
    ASSERT_ARITY_OR("newline", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("newline", args, 0, VALUE_OUTPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = car(args);
    } else {
        port = value_make_output_port(stdout);
    }

    fputc('\n', port->as.port);
    return EMPTY_LIST;
}

struct value*
builtin_write_char(struct value* args)
{
    ASSERT_ARITY_OR("write-char", args, 1, 2);
    ASSERT_TYPE("write-char", args, 0, VALUE_CHARACTER);

    long arity = list_length(args);
    if (arity == 2) {
        ASSERT_TYPE("write-char", args, 1, VALUE_OUTPUT_PORT);
    }

    struct value* obj = car(args);

    struct value* port = NULL;
    if (arity == 2) {
        port = cadr(args);
    } else {
        port = value_make_output_port(stdout);
    }

    fputc(obj->as.character, port->as.port);
    return EMPTY_LIST;
}

struct value*
builtin_sleep(struct value* args)
{
    ASSERT_ARITY("sleep!", args, 1);
    ASSERT_TYPE("sleep!", args, 0, VALUE_NUMBER);

    struct value* delay_ms = car(args);
    SDL_Delay(delay_ms->as.number);

    return EMPTY_LIST;
}

struct value*
builtin_is_window(struct value* args)
{
    ASSERT_ARITY("window?", args, 1);

    return value_is_window(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_make_window(struct value* args)
{
    ASSERT_ARITY("make-window", args, 3);
    ASSERT_TYPE("make-window", args, 0, VALUE_STRING);
    ASSERT_TYPE("make-window", args, 1, VALUE_NUMBER);
    ASSERT_TYPE("make-window", args, 2, VALUE_NUMBER);

    struct value* title = list_nth(args, 0);
    struct value* width = list_nth(args, 1);
    struct value* height = list_nth(args, 2);

    return value_make_window(title->as.string, width->as.number, height->as.number);
}

struct value*
builtin_window_clear(struct value* args)
{
    ASSERT_ARITY("window-clear!", args, 1);
    ASSERT_TYPE("window-clear!", args, 0, VALUE_WINDOW);

    struct value* window = car(args);

    SDL_SetRenderDrawColor(window->as.window.renderer, 0, 0, 0, 255);
    SDL_RenderClear(window->as.window.renderer);

    return EMPTY_LIST;
}

struct value*
builtin_window_draw_line(struct value* args)
{
    ASSERT_ARITY("window-draw-line!", args, 5);
    ASSERT_TYPE("window-draw-line!", args, 0, VALUE_WINDOW);
    ASSERT_TYPE("window-draw-line!", args, 1, VALUE_NUMBER);
    ASSERT_TYPE("window-draw-line!", args, 2, VALUE_NUMBER);
    ASSERT_TYPE("window-draw-line!", args, 3, VALUE_NUMBER);
    ASSERT_TYPE("window-draw-line!", args, 4, VALUE_NUMBER);

    struct value* window = list_nth(args, 0);
    struct value* x1 = list_nth(args, 1);
    struct value* y1 = list_nth(args, 2);
    struct value* x2 = list_nth(args, 3);
    struct value* y2 = list_nth(args, 4);

    SDL_SetRenderDrawColor(window->as.window.renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(window->as.window.renderer,
        x1->as.number, y1->as.number,
        x2->as.number, y2->as.number);

    return EMPTY_LIST;
}

struct value*
builtin_window_present(struct value* args)
{
    ASSERT_ARITY("window-present!", args, 1);
    ASSERT_TYPE("window-present!", args, 0, VALUE_WINDOW);

    struct value* window = car(args);
    SDL_RenderPresent(window->as.window.renderer);

    return EMPTY_LIST;
}

struct value*
builtin_is_event(struct value* args)
{
    ASSERT_ARITY("event?", args, 1);

    return value_is_event(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_window_event_poll(struct value* args)
{
    ASSERT_ARITY("window-event-poll", args, 1);
    ASSERT_TYPE("window-event-poll", args, 0, VALUE_WINDOW);

    struct value* events = EMPTY_LIST;
    for (;;) {
        SDL_Event* event = malloc(sizeof(SDL_Event));
        int rc = SDL_PollEvent(event);
        if (rc == 0) {
            free(event);
            break;
        } else {
            events = cons(value_make_event(event), events);
        }
    }

    return events;
}

struct value*
builtin_window_event_type(struct value* args)
{
    ASSERT_ARITY("window-event-type", args, 1);
    ASSERT_TYPE("window-event-type", args, 0, VALUE_EVENT);

    struct value* event = car(args);
    switch (event->as.event->type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            return value_make_symbol("event-keyboard");
        case SDL_MOUSEMOTION:
            return value_make_symbol("event-mousemotion");
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            return value_make_symbol("event-mousebutton");
        case SDL_QUIT:
            return value_make_symbol("event-quit");
        case SDL_WINDOWEVENT:
            return value_make_symbol("event-window");
        default:
            return value_make_symbol("event-undefined");
    }
}
