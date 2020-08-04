#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "builtin.h"
#include "env.h"
#include "mce.h"
#include "value.h"

// 1. Define core data structure (linkable tagged union)
// 2. Convert text to this data structure (read)
// 3. Evaluate the data structure (eval/apply)

// TODO: Impl alt define form for easier lambdas
// TODO: Add read_list helper to read func
// TODO: Add special form "if"
// TODO: Add assert helpers for builtins (arity and types)
// TODO: Add a simple ref counted GC / memory management

int
main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to init SDL2: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    struct value* vars = list_make(
        value_make_symbol("boolean?"),
        value_make_symbol("symbol?"),
        value_make_symbol("procedure?"),
        value_make_symbol("pair?"),
        value_make_symbol("number?"),
        value_make_symbol("string?"),
        value_make_symbol("window?"),

        value_make_symbol("+"),
        value_make_symbol("-"),
        value_make_symbol("*"),
        value_make_symbol("/"),

        value_make_symbol("delay!"),
        value_make_symbol("make-window"),
        value_make_symbol("window-clear!"),
        value_make_symbol("window-draw-line!"),
        value_make_symbol("window-present!"),

        NULL);
    struct value* vals = list_make(
        value_make_builtin(builtin_is_boolean),
        value_make_builtin(builtin_is_symbol),
        value_make_builtin(builtin_is_procedure),
        value_make_builtin(builtin_is_pair),
        value_make_builtin(builtin_is_number),
        value_make_builtin(builtin_is_string),
        value_make_builtin(builtin_is_window),

        value_make_builtin(builtin_add),
        value_make_builtin(builtin_sub),
        value_make_builtin(builtin_mul),
        value_make_builtin(builtin_div),

        value_make_builtin(builtin_delay),
        value_make_builtin(builtin_make_window),
        value_make_builtin(builtin_window_clear),
        value_make_builtin(builtin_window_draw_line),
        value_make_builtin(builtin_window_present),

        NULL);
    struct value* env = env_bind(vars, vals, NULL);

    printf("> ");
    char line[512] = { 0 };
    while (fgets(line, sizeof(line), stdin) != NULL) {
        long consumed = 0;
        struct value* exp = value_read(line, &consumed);
//        value_write(exp);
//        printf("\n");

        struct value* res = mce_eval(exp, env);
        value_write(res);
        printf("\n");

        printf("> ");
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}
