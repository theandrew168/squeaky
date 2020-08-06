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
#include "io.h"
#include "list.h"
#include "mce.h"
#include "value.h"

// 1. Define core data structure (linkable tagged union)
// 2. Convert text to this data structure (read)
// 3. Evaluate the data structure (eval/apply)

// TODO: Add type for SDL_Event and corresponding builtins
// TODO: Harden behavior for empty / incomplete / invalid expressions

// TODO: Add read_list helper to read func
// TODO: Add assert helpers for builtins (arity and types)
// TODO: Add a simple ref counted GC / memory management

#define add_to_env(sym, func, env)  \
  env_define(value_make_symbol(sym), value_make_builtin(func), env);

int
main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to init SDL2: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    struct value* env = env_extend(NULL, NULL, NULL);
    add_to_env("boolean?", builtin_is_boolean, env);
    add_to_env("number?", builtin_is_number, env);
    add_to_env("string?", builtin_is_string, env);
    add_to_env("symbol?", builtin_is_symbol, env);
    add_to_env("pair?", builtin_is_pair, env);
    add_to_env("procedure?", builtin_is_procedure, env);
    add_to_env("window?", builtin_is_window, env);
    add_to_env("event?", builtin_is_event, env);
    add_to_env("error?", builtin_is_error, env);

    add_to_env("+", builtin_add, env);
    add_to_env("-", builtin_sub, env);
    add_to_env("*", builtin_mul, env);
    add_to_env("/", builtin_div, env);
    add_to_env("=", builtin_equal, env);
    add_to_env("<", builtin_less, env);
    add_to_env("<=", builtin_less_equal, env);
    add_to_env(">", builtin_greater, env);
    add_to_env(">=", builtin_greater_equal, env);

    add_to_env("eq?", builtin_is_eq, env);
    add_to_env("not", builtin_not, env);

    add_to_env("display", builtin_display, env);
    add_to_env("newline", builtin_newline, env);

    add_to_env("delay!", builtin_delay, env);
    add_to_env("make-window", builtin_make_window, env);
    add_to_env("window-clear!", builtin_window_clear, env);
    add_to_env("window-draw-line!", builtin_window_draw_line, env);
    add_to_env("window-present!", builtin_window_present, env);
    add_to_env("event-poll", builtin_event_poll, env);
    add_to_env("event-type", builtin_event_type, env);

    printf("> ");
    char line[512] = { 0 };
    while (fgets(line, sizeof(line), stdin) != NULL) {
        // hack to re-prompt on empty input
        if (*line == '\n') {
            printf("> ");
            continue;
        }

        long consumed = 0;
        struct value* exp = io_read(line, &consumed);

        // uncomment to see what the reader gives
//        io_write(exp);
//        printf("\n");

        struct value* res = mce_eval(exp, env);
        io_write(res);
        printf("\n");

        printf("> ");
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}
