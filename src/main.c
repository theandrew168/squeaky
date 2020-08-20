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
#include "list.h"
#include "mce.h"
#include "reader.h"
#include "value.h"
#include "vm.h"

#define add_builtin(vm, sym, func, env)  \
  env_define(vm, vm_make_symbol(vm, sym), vm_make_builtin(vm, func), env)

int
main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to init SDL2: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    struct vm vm = { 0 };
    vm_init(&vm);

    struct value* env = env_empty(&vm);
    env_define(&vm, vm_make_symbol(&vm, "nil"), vm_make_empty_list(&vm), env);
    env_define(&vm, vm_make_symbol(&vm, "stdin"), vm_make_input_port(&vm, stdin), env);
    env_define(&vm, vm_make_symbol(&vm, "stdout"), vm_make_output_port(&vm, stdout), env);
    env_define(&vm, vm_make_symbol(&vm, "stderr"), vm_make_output_port(&vm, stderr), env);

    // R5RS 6.1: Equivalence Predicates
    add_builtin(&vm, "eq?", builtin_is_eq, env);  // shallow compare (slightly more specific than eqv)
    add_builtin(&vm, "eqv?", builtin_is_eqv, env);  // shallow compare (baseline "what you'd expect" comparison)
    add_builtin(&vm, "equal?", builtin_is_equal, env);  // deep compare (recursive baseline comparison)

    // R5RS 6.2.5: Numerical Operations
    add_builtin(&vm, "number?", builtin_is_number, env);
    add_builtin(&vm, "=", builtin_equal, env);
    add_builtin(&vm, "<", builtin_less, env);
    add_builtin(&vm, ">", builtin_greater, env);
    add_builtin(&vm, "<=", builtin_less_equal, env);
    add_builtin(&vm, ">=", builtin_greater_equal, env);
    add_builtin(&vm, "+", builtin_add, env);
    add_builtin(&vm, "*", builtin_mul, env);
    add_builtin(&vm, "-", builtin_sub, env);
    add_builtin(&vm, "/", builtin_div, env);

    // R5RS 6.3.1: Booleans
    add_builtin(&vm, "boolean?", builtin_is_boolean, env);

    // R5RS 6.3.2: Pairs and Lists
    add_builtin(&vm, "pair?", builtin_is_pair, env);
    add_builtin(&vm, "cons", builtin_cons, env);
    add_builtin(&vm, "car", builtin_car, env);
    add_builtin(&vm, "cdr", builtin_cdr, env);
    add_builtin(&vm, "set-car!", builtin_set_car, env);
    add_builtin(&vm, "set-cdr!", builtin_set_cdr, env);
    add_builtin(&vm, "null?", builtin_is_null, env);

    // R5RS 6.3.3: Symbols
    add_builtin(&vm, "symbol?", builtin_is_symbol, env);

    // R5RS 6.3.5: Strings
    add_builtin(&vm, "string?", builtin_is_string, env);

    // R5RS 6.4: Control Features
    add_builtin(&vm, "procedure?", builtin_is_procedure, env);
    add_builtin(&vm, "apply", mce_builtin_apply, env);  // will be handled specifically by the MCE

    // R5RS 6.5: Eval
    add_builtin(&vm, "eval", mce_builtin_eval, env);  // will be handled specifically by the MCE

    // R5RS 6.6.1: Ports
    add_builtin(&vm, "input-port?", builtin_is_input_port, env);
    add_builtin(&vm, "output-port?", builtin_is_output_port, env);
    add_builtin(&vm, "current-input-port", builtin_current_input_port, env);
    add_builtin(&vm, "current-output-port", builtin_current_output_port, env);
    add_builtin(&vm, "open-input-file", builtin_open_input_file, env);
    add_builtin(&vm, "open-output-file", builtin_open_output_file, env);
    add_builtin(&vm, "close-input-port", builtin_close_input_port, env);
    add_builtin(&vm, "close-output-port", builtin_close_output_port, env);

    // R5RS 6.6.2: Input
    add_builtin(&vm, "read", builtin_read, env);
    add_builtin(&vm, "read-char", builtin_read_char, env);
    add_builtin(&vm, "peek-char", builtin_peek_char, env);
    add_builtin(&vm, "eof-object?", builtin_is_eof_object, env);
    add_builtin(&vm, "char-ready?", builtin_is_char_ready, env);

    // R5RS 6.6.3: Output
    add_builtin(&vm, "write", builtin_write, env);
    add_builtin(&vm, "display", builtin_display, env);
    add_builtin(&vm, "newline", builtin_newline, env);
    add_builtin(&vm, "write-char", builtin_write_char, env);

    /* Squeaky Extensions */

    // General Utilities
    add_builtin(&vm, "sleep!", builtin_sleep, env);

    // Windows
    add_builtin(&vm, "window?", builtin_is_window, env);
    add_builtin(&vm, "make-window", builtin_make_window, env);
    add_builtin(&vm, "window-clear!", builtin_window_clear, env);
    add_builtin(&vm, "window-draw-line!", builtin_window_draw_line, env);
    add_builtin(&vm, "window-present!", builtin_window_present, env);

    // Events
    add_builtin(&vm, "event?", builtin_is_event, env);
    add_builtin(&vm, "window-event-poll", builtin_window_event_poll, env);
    add_builtin(&vm, "window-event-type", builtin_window_event_type, env);
    add_builtin(&vm, "window-event-key", builtin_window_event_key, env);

    // load prelude (small library of R5RS funcs and extensions)
    struct value* exp = vm_make_pair(&vm, vm_make_symbol(&vm, "load"),
                                          vm_make_pair(&vm, vm_make_string(&vm, "prelude.scm"),
                                                            vm_make_empty_list(&vm)));
    mce_eval(&vm, exp, env);

    // eval files given on CLI (if any) otherwise default to REPL
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            struct value* exp = vm_make_pair(&vm, vm_make_symbol(&vm, "load"),
                                                  vm_make_pair(&vm, vm_make_string(&vm, argv[1]),
                                                                    vm_make_empty_list(&vm)));
            mce_eval(&vm, exp, env);
        }
    } else {
        struct value* exp = vm_make_pair(&vm, vm_make_symbol(&vm, "load"),
                                              vm_make_pair(&vm, vm_make_string(&vm, "repl.scm"),
                                                                vm_make_empty_list(&vm)));
        mce_eval(&vm, exp, env);
    }

    vm_free(&vm);
    SDL_Quit();
    return EXIT_SUCCESS;
}
