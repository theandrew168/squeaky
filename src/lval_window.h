#ifndef SQUEAKY_LVAL_WINDOW_H_INCLUDED
#define SQUEAKY_LVAL_WINDOW_H_INCLUDED

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

union lval;

struct lval_window {
    int type;
    SDL_Window* window;
    SDL_GLContext context;
};

#define AS_WINDOW(val) ((struct lval_window*)(val))
#define AS_CONST_WINDOW(val) ((struct lval_window*)(val))

bool lval_window_init(union lval* val, const char* title, long width, long height);
void lval_window_free(union lval* val);
void lval_window_copy(const union lval* val, union lval* copy);

void lval_window_print(const union lval* val);
bool lval_window_equal(const union lval* a, const union lval* b);

#endif
