#ifndef SQUEAKY_LVAL_H_INCLUDED
#define SQUEAKY_LVAL_H_INCLUDED

#include <stdbool.h>

#include "lbuiltin.h"
#include "lval_error.h"
#include "lval_number.h"
#include "lval_symbol.h"
#include "lval_string.h"
#include "lval_window.h"
#include "lval_builtin.h"
#include "lval_lambda.h"
#include "lval_sexpr.h"
#include "lval_qexpr.h"

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

union lval {
    int type;
    struct lval_error error;
    struct lval_number number;
    struct lval_symbol symbol;
    struct lval_string string;
    struct lval_window window;
    struct lval_builtin builtin;
    struct lval_lambda lambda;
    struct lval_sexpr sexpr;
    struct lval_qexpr qexpr;
};

// constructors for each type
union lval* lval_make_number(long number);
union lval* lval_make_error(const char* fmt, ...);
union lval* lval_make_symbol(const char* symbol);
union lval* lval_make_string(const char* string);
union lval* lval_make_window(const char* title, long width, long height);
union lval* lval_make_builtin(lbuiltin builtin);
union lval* lval_make_lambda(union lval* formals, union lval* body);
union lval* lval_make_sexpr(void);
union lval* lval_make_qexpr(void);

// generic value operations
void lval_free(union lval* val);
union lval* lval_copy(const union lval* val);
const char* lval_type_name(int lval_type);
bool lval_equal(const union lval* a, const union lval* b);
void lval_print(const union lval* val);
void lval_println(const union lval* val);

// operations on lists (s-exprs and q-exprs)
union lval* lval_list_append(union lval* list, union lval* val);
union lval* lval_list_pop(union lval* list, long i);
union lval* lval_list_take(union lval* list, long i);
union lval* lval_list_join(union lval* list, union lval* extras);

// read a string into a list (s-expr or q-expr)
union lval* lval_read_expr(char* s, long* i, char end);

#endif
