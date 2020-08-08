#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "builtin.h"
#include "io.h"
#include "list.h"
#include "value.h"

struct value*
builtin_is_boolean(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_boolean(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_number(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_number(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_string(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_string(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_symbol(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_symbol(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_pair(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_pair(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_procedure(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_procedure(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_window(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_window(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_event(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg

    return value_is_event(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
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
builtin_equal(struct value* args)
{
    assert(args != NULL);
    // TODO: assert >= 2 args (numbers)

    struct value* item = car(args);
    for (args = cdr(args); args != NULL; args = cdr(args)) {
        if (!value_equal(item, car(args))) return value_make_boolean(false);
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
builtin_is_eq(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 2 args (any, any)

    struct value* a = car(args);
    struct value* b = cadr(args);

    return value_equal(a, b) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_not(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg (any)

    struct value* a = car(args);

    return value_is_false(a) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_display(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1-2 args (any[, port])

    struct value* obj = car(args);
    io_write(obj);

    return EMPTY_LIST;
}

struct value*
builtin_newline(struct value* args)
{
    // might actually be NULL if zero args!
//    assert(args != NULL);
    // TODO: assert 0-1 args ([port])

    printf("\n");
    return EMPTY_LIST;
}

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
