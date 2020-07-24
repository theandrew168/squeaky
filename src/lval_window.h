#ifndef SQUEAKY_LVAL_WINDOW_H_INCLUDED
#define SQUEAKY_LVAL_WINDOW_H_INCLUDED

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

struct lval;

struct lval_window {
    SDL_Window* window;
    SDL_GLContext context;
};

#define AS_WINDOW(val) ((val)->as->window)

bool lval_window_init(struct lval* val, const char* title, long width, long height);
void lval_window_free(struct lval* val);
void lval_window_copy(const struct lval* val, struct lval* copy);

void lval_window_print(const struct lval* val);
bool lval_window_equal(const struct lval* a, const struct lval* b);

#endif
