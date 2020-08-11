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

// TODO: Rewrite the reader
// TODO: Add assert helpers for builtins (arity and types)

#define add_builtin(sym, func, env)  \
  env_define(value_make_symbol(sym), value_make_builtin(func), env);

int
main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to init SDL2: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    struct value* env = env_extend(NULL, NULL, NULL);
    env_define(value_make_symbol("stdin"), value_make_input_port(stdin), env);
    env_define(value_make_symbol("stdout"), value_make_output_port(stdout), env);
    env_define(value_make_symbol("stderr"), value_make_output_port(stderr), env);

    // R5RS 6.1: Equivalence Predicates
    add_builtin("eq?", builtin_is_eq, env);  // shallow compare (slightly more specific than eqv)
    add_builtin("eqv?", builtin_is_eqv, env);  // shallow compare (baseline "what you'd expect" comparison)
    add_builtin("equal?", builtin_is_equal, env);  // deep compare (recursive baseline comparison)

    // R5RS 6.2.5: Numerical Operations
    add_builtin("number?", builtin_is_number, env);
    add_builtin("=", builtin_equal, env);
    add_builtin("<", builtin_less, env);
    add_builtin(">", builtin_greater, env);
    add_builtin("<=", builtin_less_equal, env);
    add_builtin(">=", builtin_greater_equal, env);
    add_builtin("+", builtin_add, env);
    add_builtin("*", builtin_mul, env);
    add_builtin("-", builtin_sub, env);
    add_builtin("/", builtin_div, env);

    // R5RS 6.3.1: Booleans
    add_builtin("boolean?", builtin_is_boolean, env);

    // R5RS 6.3.2: Pairs and Lists
    add_builtin("pair?", builtin_is_pair, env);
    add_builtin("cons", builtin_cons, env);
    add_builtin("car", builtin_car, env);
    add_builtin("cdr", builtin_cdr, env);
    add_builtin("set-car!", builtin_set_car, env);
    add_builtin("set-cdr!", builtin_set_cdr, env);
    add_builtin("null?", builtin_is_null, env);

    // R5RS 6.3.3: Symbols
    add_builtin("symbol?", builtin_is_symbol, env);

    // R5RS 6.3.5: Strings
    add_builtin("string?", builtin_is_string, env);

    // R5RS 6.4: Control Features
    add_builtin("procedure?", builtin_is_procedure, env);

    // R5RS 6.6.1: Ports
    add_builtin("input-port?", builtin_is_input_port, env);
    add_builtin("output-port?", builtin_is_output_port, env);
    add_builtin("current-input-port", builtin_current_input_port, env);
    add_builtin("current-output-port", builtin_current_output_port, env);
    add_builtin("open-input-file", builtin_open_input_file, env);
    add_builtin("open-output-file", builtin_open_output_file, env);
    add_builtin("close-input-port", builtin_close_input_port, env);
    add_builtin("close-output-port", builtin_close_output_port, env);

    // R5RS 6.6.2: Input
    add_builtin("read", builtin_read, env);
    add_builtin("read-char", builtin_read_char, env);
    add_builtin("peek-char", builtin_peek_char, env);
    add_builtin("eof-object?", builtin_is_eof_object, env);
    add_builtin("char-ready?", builtin_is_char_ready, env);

//    // R5RS 6.6.3: Output
//    add_builtin("write", builtin_write, env);
//    add_builtin("display", builtin_display, env);
//    add_builtin("newline", builtin_newline, env);
//    add_builtin("write-char", builtin_write_char, env);

    /* Squeaky Extensions */

    // General Utilities
    add_builtin("sleep!", builtin_sleep, env);

    // Windows
    add_builtin("window?", builtin_is_window, env);
    add_builtin("make-window", builtin_make_window, env);
    add_builtin("window-clear!", builtin_window_clear, env);
    add_builtin("window-draw-line!", builtin_window_draw_line, env);
    add_builtin("window-present!", builtin_window_present, env);

    // Events
    add_builtin("event?", builtin_is_event, env);
    add_builtin("window-event-poll", builtin_window_event_poll, env);
    add_builtin("window-event-type", builtin_window_event_type, env);

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

        // uncomment to see what the reader reads
        io_writeln(exp);

        struct value* res = mce_eval(exp, env);
        io_writeln(res);

        printf("> ");
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}
