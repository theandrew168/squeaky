#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "lval.h"
#include "lval_window.h"

bool
lval_window_init(union lval* val, const char* title, long width, long height)
{
    assert(val != NULL);
    assert(title != NULL);

    struct lval_window* v = AS_WINDOW(val);

    // Request at least 32-bit color
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Request a double-buffered, OpenGL 3.3 (or higher) core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL);

    if (window == NULL) {
        fprintf(stderr, "failed to create SDL2 window: %s", SDL_GetError());
        return false;
    }

    // SDL_GLContext is an alias for void*
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "failed to create OpenGL context: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetSwapInterval(1);

    v->type = LVAL_TYPE_WINDOW;
    v->window = window;
    v->context = context;

    return true;
}

void
lval_window_free(union lval* val)
{
    assert(val != NULL);

//    struct lval_window* v = AS_WINDOW(val);

    // TODO: ref count deez boiz
    // otherwise any copy new + free old deletes the window :(
//    SDL_GL_DeleteContext(val->context);
//    SDL_DestroyWindow(val->window);
}

void
lval_window_copy(const union lval* val, union lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    const struct lval_window* v = AS_CONST_WINDOW(val);
    struct lval_window* c = AS_WINDOW(copy);

    c->type = v->type;
    c->window = v->window;
    c->context = v->context;
}

void
lval_window_print(const union lval* val)
{
    assert(val != NULL);

    printf("<window>");
}

bool
lval_window_equal(const union lval* a, const union lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    const struct lval_window* aa = AS_CONST_WINDOW(a);
    const struct lval_window* bb = AS_CONST_WINDOW(b);

    return aa->window == bb->window && aa->context == bb->context;
}
