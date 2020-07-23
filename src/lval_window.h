#ifndef SQUEAKY_LVAL_WINDOW_H_INCLUDED
#define SQUEAKY_LVAL_WINDOW_H_INCLUDED

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

struct lval_window {
    int type;
    SDL_Window* window;
    SDL_GLContext context;
};

bool lval_window_init(struct lval_window* val, const char* title, long width, long height);
void lval_window_free(struct lval_window* val);
void lval_window_copy(const struct lval_window* val, struct lval_window* copy);

void lval_window_print(const struct lval_window* val);
bool lval_window_equal(const struct lval_window* a, const struct lval_window* b);

#endif
