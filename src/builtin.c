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
#include "vm.h"

struct value*
builtin_is_eq(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("eq?", args, 2);

    struct value* a = CAR(args);
    struct value* b = CADR(args);

    return value_is_eq(a, b) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_eqv(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("eqv?", args, 2);

    struct value* a = CAR(args);
    struct value* b = CADR(args);

    return value_is_eqv(a, b) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_equal(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("equal?", args, 2);

    struct value* a = CAR(args);
    struct value* b = CADR(args);

    return value_is_equal(a, b) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_number(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("number?", args, 1);

    return value_is_number(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_equal(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_GTE("=", args, 2);
    ASSERT_TYPE_ALL("=", args, VALUE_NUMBER);

    struct value* item = CAR(args);
    args = CDR(args);

    while (!value_is_empty_list(args)) {
        if (!value_is_equal(item, CAR(args))) return vm_make_boolean(vm, false);
        item = CAR(args);
        args = CDR(args);
    }

    return vm_make_boolean(vm, true);
}

struct value*
builtin_less(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_GTE("<", args, 2);
    ASSERT_TYPE_ALL("<", args, VALUE_NUMBER);

    struct value* item = CAR(args);
    args = CDR(args);

    while (!value_is_empty_list(args)) {
        if (!(item->as.number < CAR(args)->as.number)) return vm_make_boolean(vm, false);
        item = CAR(args);
        args = CDR(args);
    }

    return vm_make_boolean(vm, true);
}

struct value*
builtin_greater(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_GTE(">", args, 2);
    ASSERT_TYPE_ALL(">", args, VALUE_NUMBER);

    struct value* item = CAR(args);
    args = CDR(args);

    while (!value_is_empty_list(args)) {
        if (!(item->as.number > CAR(args)->as.number)) return vm_make_boolean(vm, false);
        item = CAR(args);
        args = CDR(args);
    }

    return vm_make_boolean(vm, true);
}

struct value*
builtin_less_equal(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_GTE("<=", args, 2);
    ASSERT_TYPE_ALL("<=", args, VALUE_NUMBER);

    struct value* item = CAR(args);
    args = CDR(args);

    while (!value_is_empty_list(args)) {
        if (!(item->as.number <= CAR(args)->as.number)) return vm_make_boolean(vm, false);
        item = CAR(args);
        args = CDR(args);
    }

    return vm_make_boolean(vm, true);
}

struct value*
builtin_greater_equal(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_GTE(">=", args, 2);
    ASSERT_TYPE_ALL(">=", args, VALUE_NUMBER);

    struct value* item = CAR(args);
    args = CDR(args);

    while (!value_is_empty_list(args)) {
        if (!(item->as.number >= CAR(args)->as.number)) return vm_make_boolean(vm, false);
        item = CAR(args);
        args = CDR(args);
    }

    return vm_make_boolean(vm, true);
}

struct value*
builtin_add(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_GTE("+", args, 2);
    ASSERT_TYPE_ALL("+", args, VALUE_NUMBER);

    long res = 0;
    while (!value_is_empty_list(args)) {
        res += CAR(args)->as.number;
        args = CDR(args);
    }

    return vm_make_number(vm, res);
}

struct value*
builtin_mul(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_GTE("*", args, 2);
    ASSERT_TYPE_ALL("*", args, VALUE_NUMBER);

    long res = 1;
    while (!value_is_empty_list(args)) {
        res *= CAR(args)->as.number;
        args = CDR(args);
    }

    return vm_make_number(vm, res);
}

struct value*
builtin_sub(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_GTE("-", args, 2);
    ASSERT_TYPE_ALL("-", args, VALUE_NUMBER);

    long res = CAR(args)->as.number;
    args = CDR(args);

    while (!value_is_empty_list(args)) {
        res -= CAR(args)->as.number;
        args = CDR(args);
    }

    return vm_make_number(vm, res);
}

struct value*
builtin_div(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_GTE("/", args, 2);
    ASSERT_TYPE_ALL("/", args, VALUE_NUMBER);

    long res = CAR(args)->as.number;
    args = CDR(args);

    while (!value_is_empty_list(args)) {
        if (CAR(args)->as.number == 0) {
            fprintf(stderr, "error: divide by zero\n");
            exit(EXIT_FAILURE);
        }

        res /= CAR(args)->as.number;
        args = CDR(args);
    }

    return vm_make_number(vm, res);
}

struct value*
builtin_is_boolean(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("boolean?", args, 1);

    return value_is_boolean(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_pair(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("pair?", args, 1);

    return value_is_pair(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_cons(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("cons", args, 2);

    struct value* a = CAR(args);
    struct value* b = CADR(args);
    return vm_make_pair(vm, a, b);
}

struct value*
builtin_car(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("car", args, 1);
    ASSERT_TYPE("car", args, 0, VALUE_PAIR);

    struct value* pair = CAR(args);
    return CAR(pair);
}

struct value*
builtin_cdr(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("cdr", args, 1);
    ASSERT_TYPE("cdr", args, 0, VALUE_PAIR);

    struct value* pair = CAR(args);
    return CDR(pair);
}

struct value*
builtin_set_car(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("set-car!", args, 2);
    ASSERT_TYPE("set-car!", args, 0, VALUE_PAIR);

    struct value* pair = CAR(args);
    struct value* val = CADR(args);
    pair->as.pair.car = val;
    return vm_make_empty_list(vm);
}

struct value*
builtin_set_cdr(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("set-cdr!", args, 2);
    ASSERT_TYPE("set-cdr!", args, 0, VALUE_PAIR);

    struct value* pair = CAR(args);
    struct value* val = CADR(args);
    pair->as.pair.cdr = val;
    return vm_make_empty_list(vm);
}

struct value*
builtin_is_null(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("null?", args, 1);

    return value_is_empty_list(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_symbol(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("symbol?", args, 1);

    return value_is_symbol(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_string(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("string?", args, 1);

    return value_is_string(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_procedure(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("procedure?", args, 1);

    return value_is_procedure(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_input_port(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("input-port?", args, 1);

    return value_is_input_port(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_output_port(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("output-port?", args, 1);

    return value_is_output_port(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

// TODO: is there a better way to handle this?
struct value*
builtin_current_input_port(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("current-input-port", args, 0);

    return vm_make_input_port(vm, stdin);
}

// TODO: is there a better way to handle this?
struct value*
builtin_current_output_port(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("current-output-port", args, 0);

    return vm_make_output_port(vm, stdout);
}

struct value*
builtin_open_input_file(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("open-input-file", args, 1);
    ASSERT_TYPE("open-input-file", args, 0, VALUE_STRING);

    struct value* path = CAR(args);

    FILE* port = fopen(path->as.string, "rb");
    if (port == NULL) {
        fprintf(stderr, "failed to open input file: %s\n", path->as.string);
        perror("reason");
        exit(EXIT_FAILURE);
    }

    return vm_make_input_port(vm, port);
}

struct value*
builtin_open_output_file(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("open-output-file", args, 1);
    ASSERT_TYPE("open-output-file", args, 0, VALUE_STRING);

    struct value* path = CAR(args);

    FILE* port = fopen(path->as.string, "wb");
    if (port == NULL) {
        fprintf(stderr, "failed to open output file: %s\n", path->as.string);
        perror("reason");
        exit(EXIT_FAILURE);
    }

    return vm_make_output_port(vm, port);
}

struct value*
builtin_close_input_port(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("close-input-port", args, 1);
    ASSERT_TYPE("close-input-port", args, 0, VALUE_INPUT_PORT);

    struct value* port = CAR(args);
    fclose(port->as.port);

    return vm_make_empty_list(vm);
}

struct value*
builtin_close_output_port(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("close-output-port", args, 1);
    ASSERT_TYPE("close-output-port", args, 0, VALUE_OUTPUT_PORT);

    struct value* port = CAR(args);
    fclose(port->as.port);

    return vm_make_empty_list(vm);
}

struct value*
builtin_read(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_OR("read", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("read", args, 0, VALUE_INPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = CAR(args);
    } else {
        port = vm_make_input_port(vm, stdin);
    }

    FILE* fp = port->as.port;
    return reader_read(vm, fp);
}

struct value*
builtin_read_char(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_OR("read-char", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("read-char", args, 0, VALUE_INPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = CAR(args);
    } else {
        port = vm_make_input_port(vm, stdin);
    }

    FILE* fp = port->as.port;

    int c = fgetc(fp);
    if (ferror(fp)) {
        perror("error reading from port");
        exit(EXIT_FAILURE);
    }

    if (c == EOF && feof(fp)) {
        return vm_make_eof(vm);
    }

    return vm_make_character(vm, c);
}

struct value*
builtin_peek_char(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_OR("peek-char", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("peek-char", args, 0, VALUE_INPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = CAR(args);
    } else {
        port = vm_make_input_port(vm, stdin);
    }

    FILE* fp = port->as.port;

    int c = fgetc(fp);
    if (ferror(fp)) {
        perror("error reading from port");
        exit(EXIT_FAILURE);
    }

    if (c == EOF && feof(fp)) {
        return vm_make_eof(vm);
    }

    ungetc(c, fp);

    return vm_make_character(vm, c);
}

struct value*
builtin_is_eof_object(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("eof-object?", args, 1);

    return value_is_eof(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_is_char_ready(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_OR("char-ready?", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("char-ready?", args, 0, VALUE_INPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = CAR(args);
    } else {
        port = vm_make_input_port(vm, stdin);
    }

    FILE* fp = port->as.port;

    int c = fgetc(fp);
    if (ferror(fp)) {
        perror("error reading from port");
        exit(EXIT_FAILURE);
    }

    if (c == EOF && feof(fp)) {
        return vm_make_boolean(vm, true);
    }

    ungetc(c, fp);

    return vm_make_boolean(vm, true);
}

struct value*
builtin_write(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_OR("write", args, 1, 2);

    long arity = list_length(args);
    if (arity == 2) {
        ASSERT_TYPE("write", args, 1, VALUE_OUTPUT_PORT);
    }

    struct value* obj = CAR(args);

    struct value* port = NULL;
    if (arity == 2) {
        port = CADR(args);
    } else {
        port = vm_make_output_port(vm, stdout);
    }

    value_print(port->as.port, obj);
    return vm_make_empty_list(vm);
}

struct value*
builtin_display(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_OR("display", args, 1, 2);

    long arity = list_length(args);
    if (arity == 2) {
        ASSERT_TYPE("display", args, 1, VALUE_OUTPUT_PORT);
    }

    struct value* obj = CAR(args);

    struct value* port = NULL;
    if (arity == 2) {
        port = CADR(args);
    } else {
        port = vm_make_output_port(vm, stdout);
    }

    value_print(port->as.port, obj);
    return vm_make_empty_list(vm);
}

struct value*
builtin_newline(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_OR("newline", args, 0, 1);

    long arity = list_length(args);
    if (arity == 1) {
        ASSERT_TYPE("newline", args, 0, VALUE_OUTPUT_PORT);
    }

    struct value* port = NULL;
    if (arity == 1) {
        port = CAR(args);
    } else {
        port = vm_make_output_port(vm, stdout);
    }

    fputc('\n', port->as.port);
    return vm_make_empty_list(vm);
}

struct value*
builtin_write_char(struct vm* vm, struct value* args)
{
    ASSERT_ARITY_OR("write-char", args, 1, 2);
    ASSERT_TYPE("write-char", args, 0, VALUE_CHARACTER);

    long arity = list_length(args);
    if (arity == 2) {
        ASSERT_TYPE("write-char", args, 1, VALUE_OUTPUT_PORT);
    }

    struct value* obj = CAR(args);

    struct value* port = NULL;
    if (arity == 2) {
        port = CADR(args);
    } else {
        port = vm_make_output_port(vm, stdout);
    }

    fputc(obj->as.character, port->as.port);
    return vm_make_empty_list(vm);
}

struct value*
builtin_sleep(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("sleep!", args, 1);
    ASSERT_TYPE("sleep!", args, 0, VALUE_NUMBER);

    struct value* delay_ms = CAR(args);
    SDL_Delay(delay_ms->as.number);

    return vm_make_empty_list(vm);
}

struct value*
builtin_is_window(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("window?", args, 1);

    return value_is_window(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_make_window(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("make-window", args, 3);
    ASSERT_TYPE("make-window", args, 0, VALUE_STRING);
    ASSERT_TYPE("make-window", args, 1, VALUE_NUMBER);
    ASSERT_TYPE("make-window", args, 2, VALUE_NUMBER);

    struct value* title = list_nth(args, 0);
    struct value* width = list_nth(args, 1);
    struct value* height = list_nth(args, 2);

    return vm_make_window(vm, title->as.string, width->as.number, height->as.number);
}

struct value*
builtin_window_clear(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("window-clear!", args, 1);
    ASSERT_TYPE("window-clear!", args, 0, VALUE_WINDOW);

    struct value* window = CAR(args);

    SDL_SetRenderDrawColor(window->as.window.renderer, 0, 0, 0, 255);
    SDL_RenderClear(window->as.window.renderer);

    return vm_make_empty_list(vm);
}

struct value*
builtin_window_draw_line(struct vm* vm, struct value* args)
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

    return vm_make_empty_list(vm);
}

struct value*
builtin_window_present(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("window-present!", args, 1);
    ASSERT_TYPE("window-present!", args, 0, VALUE_WINDOW);

    struct value* window = CAR(args);
    SDL_RenderPresent(window->as.window.renderer);

    return vm_make_empty_list(vm);
}

struct value*
builtin_is_event(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("event?", args, 1);

    return value_is_event(CAR(args)) ? vm_make_boolean(vm, true) : vm_make_boolean(vm, false);
}

struct value*
builtin_window_event_poll(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("window-event-poll", args, 1);
    ASSERT_TYPE("window-event-poll", args, 0, VALUE_WINDOW);

    SDL_Event* event = malloc(sizeof(SDL_Event));
    int rc = SDL_PollEvent(event);
    if (rc == 0) {
        free(event);
        return vm_make_empty_list(vm);
    } else {
        return vm_make_event(vm, event);
    }
}

struct value*
builtin_window_event_type(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("window-event-type", args, 1);
    ASSERT_TYPE("window-event-type", args, 0, VALUE_EVENT);

    struct value* event = CAR(args);
    switch (event->as.event->type) {
        case SDL_KEYDOWN:
//        case SDL_KEYUP:
            return vm_make_symbol(vm, "event-keyboard");
        case SDL_MOUSEMOTION:
            return vm_make_symbol(vm, "event-mousemotion");
//        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            return vm_make_symbol(vm, "event-mousebutton");
        case SDL_QUIT:
            return vm_make_symbol(vm, "event-quit");
        case SDL_WINDOWEVENT:
            return vm_make_symbol(vm, "event-window");
        default:
            return vm_make_symbol(vm, "event-undefined");
    }
}

struct value*
builtin_window_event_key(struct vm* vm, struct value* args)
{
    ASSERT_ARITY("window-event-type", args, 1);
    ASSERT_TYPE("window-event-type", args, 0, VALUE_EVENT);

    struct value* event = CAR(args);
    switch (event->as.event->key.keysym.sym) {
        case SDLK_ESCAPE:
            return vm_make_symbol(vm, "key-escape");
        case SDLK_LEFT:
            return vm_make_symbol(vm, "key-left");
        case SDLK_RIGHT:
            return vm_make_symbol(vm, "key-right");
        default:
            return vm_make_symbol(vm, "key-undefined");
    }
}
