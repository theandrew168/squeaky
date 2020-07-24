#ifndef SQUEAKY_LVAL_H_INCLUDED
#define SQUEAKY_LVAL_H_INCLUDED

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "lbuiltin.h"
#include "lval_builtin.h"
#include "lval_error.h"
#include "lval_lambda.h"
#include "lval_list.h"
#include "lval_number.h"
#include "lval_string.h"
#include "lval_symbol.h"
#include "lval_window.h"

enum lval_type {
    LVAL_TYPE_UNDEFINED = 0,
    LVAL_TYPE_ERROR,
    LVAL_TYPE_NUMBER,
    LVAL_TYPE_SYMBOL,
    LVAL_TYPE_STRING,
    LVAL_TYPE_WINDOW,
    LVAL_TYPE_BUILTIN,
    LVAL_TYPE_LAMBDA,
    LVAL_TYPE_SEXPR,
    LVAL_TYPE_QEXPR,
};

// what is a better way to handle the list types? (sexpr and qexpr)
// plenty of eval-related code assumes lists of some sort.
// all of the AS_FOO casting gets a bit obnoxious
// does it make for sense for every value to be linkable?
// or rather to expose linked listing without punning?
// or should all list-accepting calls accept something more specific?
// sexpr and qexpr are treated exactly the same except for in eval?

struct lval {
    int type;
    union {
        char* error;
        long number;
        char* symbol;
        char* string;
        struct {
            SDL_Window* window;
            SDL_GLContext context;
        } window;
        lbuiltin builtin;
        struct {
            struct lenv* env;
            struct lval* formals;
            struct lval* body;
        } lambda;
    } as;

    long count;
    struct lval** list;
};

// constructors for each type
struct lval* lval_make_number(long number);
struct lval* lval_make_error(const char* fmt, ...);
struct lval* lval_make_symbol(const char* symbol);
struct lval* lval_make_string(const char* string);
struct lval* lval_make_window(const char* title, long width, long height);
struct lval* lval_make_builtin(lbuiltin builtin);
struct lval* lval_make_lambda(struct lval* formals, struct lval* body);
struct lval* lval_make_sexpr(void);
struct lval* lval_make_qexpr(void);

// generic value operations
void lval_free(struct lval* val);
struct lval* lval_copy(const struct lval* val);
const char* lval_type_name(int lval_type);
bool lval_equal(const struct lval* a, const struct lval* b);
void lval_print(const struct lval* val);
void lval_println(const struct lval* val);

// read a string into a list (s-expr or q-expr)
struct lval* lval_read_expr(char* s, long* i, char end);

#endif
