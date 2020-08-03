#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "builtin.h"
#include "value.h"

struct value*
builtin_is_boolean(struct value* args)
{
    assert(args != NULL);

    return value_is_boolean(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_symbol(struct value* args)
{
    assert(args != NULL);

    return value_is_symbol(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_procedure(struct value* args)
{
    assert(args != NULL);

    return value_is_procedure(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_pair(struct value* args)
{
    assert(args != NULL);

    return value_is_pair(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_number(struct value* args)
{
    assert(args != NULL);

    return value_is_number(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_string(struct value* args)
{
    assert(args != NULL);

    return value_is_string(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_is_window(struct value* args)
{
    assert(args != NULL);

    return value_is_window(car(args)) ? value_make_boolean(true) : value_make_boolean(false);
}

struct value*
builtin_delay(struct value* args)
{
    assert(args != NULL);
    // TODO: assert 1 arg (number)

    struct value* delay_ms = car(args);

    SDL_Delay(delay_ms->as.number);

    return value_make_pair(NULL, NULL);
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
    SDL_RenderPresent(window->as.window.renderer);

    return value_make_pair(NULL, NULL);
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
    SDL_RenderPresent(window->as.window.renderer);

    return value_make_pair(NULL, NULL);
}

struct value*
builtin_plus(struct value* args)
{
    assert(args != NULL);

    long sum = 0;
    for (; args != NULL; args = cdr(args)) {
        sum += car(args)->as.number;
    }

    return value_make_number(sum);
}

struct value*
builtin_multiply(struct value* args)
{
    assert(args != NULL);

    long product = 1;
    for (; args != NULL; args = cdr(args)) {
        product *= car(args)->as.number;
    }

    return value_make_number(product);
}
