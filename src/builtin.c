#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "builtin.h"
#include "list.h"
#include "value.h"

struct value*
builtin_is_eq(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 2 args (any, any)

    struct value* a = car(args);
    struct value* b = cadr(args);

    return value_is_eq(a, b) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_eqv(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 2 args (any, any)

    struct value* a = car(args);
    struct value* b = cadr(args);

    return value_is_eqv(a, b) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_equal(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 2 args (any, any)

    struct value* a = car(args);
    struct value* b = cadr(args);

    return value_is_equal(a, b) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_number(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_number(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_equal(struct value* args)
{
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)

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
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)

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
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)

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
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)

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
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)

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
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)

    long res = 0;
    for (; args != NULL; args = cdr(args)) {
        res += car(args)->as.number;
    }

    return value_make_number(res);
}

struct value*
builtin_mul(struct value* args)
{
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)

    long res = 1;
    for (; args != NULL; args = cdr(args)) {
        res *= car(args)->as.number;
    }

    return value_make_number(res);
}

struct value*
builtin_sub(struct value* args)
{
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)

    long res = car(args)->as.number;
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        res -= car(args)->as.number;
    }

    return value_make_number(res);
}

struct value*
builtin_div(struct value* args)
{
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)
    // TODO: assert non-zero args (except first as special case ret zero?)

    long res = car(args)->as.number;
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        res /= car(args)->as.number;
    }

    return value_make_number(res);
}

struct value*
builtin_is_boolean(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_boolean(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_pair(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_pair(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_cons(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 2 args (any, any)

    struct value* a = car(args);
    struct value* b = cadr(args);
    return cons(a, b);
}

struct value*
builtin_car(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg (pair)

    struct value* pair = car(args);
    return car(pair);
}

struct value*
builtin_cdr(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg (pair)

    struct value* pair = car(args);
    return cdr(pair);
}

struct value*
builtin_set_car(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 2 args (pair, any)

    struct value* pair = car(args);
    struct value* val = cadr(args);
    pair->as.pair.car = val;
    return EMPTY_LIST;
}

struct value*
builtin_set_cdr(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 2 args (pair, any)

    struct value* pair = car(args);
    struct value* val = cadr(args);
    pair->as.pair.cdr = val;
    return EMPTY_LIST;
}

struct value*
builtin_is_null(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return car(args) == EMPTY_LIST ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_symbol(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_symbol(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_string(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_string(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_procedure(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_procedure(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_input_port(struct value* args)
{
    assert(args != NULL);
    return value_is_input_port(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_output_port(struct value* args)
{
    assert(args != NULL);
    return value_is_output_port(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

// TODO: is there a better way to handle this?
struct value*
builtin_current_input_port(struct value* args)
{
//    assert(args != NULL);
    return value_make_input_port(stdin);
}

// TODO: is there a better way to handle this?
struct value*
builtin_current_output_port(struct value* args)
{
//    assert(args != NULL);
    return value_make_output_port(stdout);
}

struct value*
builtin_open_input_file(struct value* args)
{
    assert(args != NULL);

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
    assert(args != NULL);

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
    assert(args != NULL);

    struct value* port = car(args);
    fclose(port->as.port);

    return EMPTY_LIST;
}

struct value*
builtin_close_output_port(struct value* args)
{
    assert(args != NULL);

    struct value* port = car(args);
    fclose(port->as.port);

    return EMPTY_LIST;
}

struct value*
builtin_read(struct value* args)
{
    struct value* port = NULL;
    if (args == EMPTY_LIST) {
        port = builtin_current_input_port(args);
    } else {
        port = car(args);
    }

//    FILE* fp = port->as.port;
//    return reader_read(fp);
    return EMPTY_LIST;
}

struct value*
builtin_read_char(struct value* args)
{
    struct value* port = NULL;
    if (args == EMPTY_LIST) {
        port = builtin_current_input_port(args);
    } else {
        port = car(args);
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
    struct value* port = NULL;
    if (args == EMPTY_LIST) {
        port = builtin_current_input_port(args);
    } else {
        port = car(args);
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
    assert(args != NULL);
    // assrts 1 arg (any)

    return value_is_eof(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_char_ready(struct value* args)
{
    struct value* port = NULL;
    if (args == EMPTY_LIST) {
        port = builtin_current_input_port(args);
    } else {
        port = car(args);
    }

    FILE* fp = port->as.port;

    int c = fgetc(fp);
    if (ferror(fp)) {
        perror("error reading from port");
        exit(EXIT_FAILURE);
    }

    if (c == EOF && feof(fp)) {
        return value_make_boolean(false);
    }

    ungetc(c, fp);

    return value_make_boolean(true);
}

struct value*
builtin_write(struct value* args)
{
    assert(list_length(args) == 1 || list_length(args) == 2);

    struct value* obj = car(args);

    struct value* port = NULL;
    if (cddr(args) == EMPTY_LIST) {
        port = builtin_current_output_port(args);
    } else {
        port = cadr(args);
    }

    // TODO: actually do the write
    return EMPTY_LIST;
}

struct value*
builtin_display(struct value* args)
{
    assert(args != NULL);
    return EMPTY_LIST;
}

struct value*
builtin_newline(struct value* args)
{
    assert(args != NULL);
    return EMPTY_LIST;
}

struct value*
builtin_write_char(struct value* args)
{
    assert(args != NULL);
    return EMPTY_LIST;
}

//struct value*
//builtin_display(struct value* args)
//{
//    assert(args != NULL);
//    // TODO: assert 1-2 args (any[, port])
//
//    struct value* obj = car(args);
//    io_write(obj);
//
//    return EMPTY_LIST;
//}
//
//struct value*
//builtin_newline(struct value* args)
//{
//    // might actually be NULL if zero args!
////    assert(args != NULL);
//    // TODO: assert 0-1 args ([port])
//
//    printf("\n");
//    return EMPTY_LIST;
//}

struct value*
builtin_sleep(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg (number)

    struct value* delay_ms = car(args);

    SDL_Delay(delay_ms->as.number);

    return EMPTY_LIST;
}

struct value*
builtin_is_window(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_window(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_make_window(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 3 args (string, number, number)
    // TODO: nth helper?

    struct value* title = car(args);
    struct value* width = cadr(args);
    struct value* height = caddr(args);

    return value_make_window(title->as.string, width->as.number, height->as.number);
}

struct value*
builtin_window_clear(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg (window)

    struct value* window = car(args);

    SDL_SetRenderDrawColor(window->as.window.renderer, 0, 0, 0, 255);
    SDL_RenderClear(window->as.window.renderer);

    return EMPTY_LIST;
}

struct value*
builtin_window_draw_line(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 5 arg (window, 4x number)

    struct value* window = car(args);
    struct value* x1 = cadr(args);
    struct value* y1 = caddr(args);
    struct value* x2 = caddr(cdr(args));
    struct value* y2 = caddr(cddr(args));

    SDL_SetRenderDrawColor(window->as.window.renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(window->as.window.renderer,
        x1->as.number, y1->as.number,
        x2->as.number, y2->as.number);

    return EMPTY_LIST;
}

struct value*
builtin_window_present(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg (window)

    struct value* window = car(args);

    SDL_RenderPresent(window->as.window.renderer);

    return EMPTY_LIST;
}

struct value*
builtin_is_event(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_event(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_window_event_poll(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg (window)

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
    assert(args != NULL);
    // TODO: assert 1 arg (event)

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
