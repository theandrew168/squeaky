#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "lval.h"
#include "lval_window.h"

bool
lval_window_init(struct lval* val, const char* title, long width, long height)
{
    assert(val != NULL);
    assert(title != NULL);

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

    val->type = LVAL_TYPE_WINDOW;
    val->as.window.window = window;
    val->as.window.context = context;

    return true;
}

void
lval_window_free(struct lval* val)
{
    assert(val != NULL);

    // TODO: ref count deez boiz
    // otherwise any copy new + free old deletes the window :(
//    SDL_GL_DeleteContext(val->as.window.context);
//    SDL_DestroyWindow(val->as.window.window);
}

void
lval_window_copy(const struct lval* val, struct lval* copy)
{
    assert(val != NULL);
    assert(copy != NULL);

    copy->type = val->type;
    copy->as.window.window = val->as.window.window;
    copy->as.window.context = val->as.window.context;
}

void
lval_window_print(const struct lval* val)
{
    assert(val != NULL);

    printf("<window>");
}

bool
lval_window_equal(const struct lval* a, const struct lval* b)
{
    assert(a != NULL);
    assert(b != NULL);

    return a->as.window.window == b->as.window.window &&
           a->as.window.context == b->as.window.context;
}
