#ifndef SQUEAKY_VALUE_H_INCLUDED
#define SQUEAKY_VALUE_H_INCLUDED

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

enum value_type {
    VALUE_UNDEFINED = 0,
    VALUE_BOOLEAN,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_SYMBOL,
    VALUE_PAIR,
    VALUE_BUILTIN,
    VALUE_LAMBDA,
    VALUE_WINDOW,
    VALUE_EVENT,
    VALUE_ERROR,
};

struct value;
typedef struct value* (*builtin_func)(struct value* args);

struct value {
    int type;
    int ref_count;
    union {
        bool boolean;
        long number;
        char* string;
        char* symbol;
        struct {
            struct value* car;
            struct value* cdr;
        } pair;
        builtin_func builtin;
        struct {
            struct value* params;
            struct value* body;
            struct value* env;
        } lambda;
        struct {
            SDL_Window* window;
            SDL_Renderer* renderer;
        } window;
        SDL_Event event;
        char* error;
    } as;
};

#define value_is_boolean(v) ((v)->type == VALUE_BOOLEAN)
#define value_is_number(v)  ((v)->type == VALUE_NUMBER)
#define value_is_string(v)  ((v)->type == VALUE_STRING)
#define value_is_symbol(v)  ((v)->type == VALUE_SYMBOL)
#define value_is_pair(v)    ((v)->type == VALUE_PAIR)
#define value_is_builtin(v) ((v)->type == VALUE_BUILTIN)
#define value_is_lambda(v)  ((v)->type == VALUE_LAMBDA)
#define value_is_window(v)  ((v)->type == VALUE_WINDOW)
#define value_is_event(v)   ((v)->type == VALUE_EVENT)
#define value_is_error(v)   ((v)->type == VALUE_ERROR)
bool value_is_true(const struct value* exp);
bool value_is_false(const struct value* exp);
bool value_is_procedure(const struct value* exp);

// constructors
struct value* value_make_boolean(bool boolean);
struct value* value_make_number(long number);
struct value* value_make_string(const char* string);
struct value* value_make_stringn(const char* string, long length);
struct value* value_make_symbol(const char* symbol);
struct value* value_make_symboln(const char* symbol, long length);
struct value* value_make_pair(struct value* car, struct value* cdr);
struct value* value_make_builtin(builtin_func builtin);
struct value* value_make_lambda(struct value* params, struct value* body, struct value* env);
struct value* value_make_window(const char* title, long width, long height);
struct value* value_make_event(SDL_Event event);
struct value* value_make_error(const char* error);
void value_free(struct value* value);

// ref count functions
void value_ref_inc(struct value* value);
void value_ref_dec(struct value* value);

// extra goodies
bool value_equal(const struct value* a, const struct value* b);

// read / write
struct value* value_read(const char* str, long* consumed);
void value_write(const struct value* value);

// list functions
struct value* list_make(struct value* value, ...);
long list_length(const struct value* list);
bool list_is_null(const struct value* list);
struct value* list_car(const struct value* list);
struct value* list_cdr(const struct value* list);

#define cons(a,b) (value_make_pair((a), (b)))

#define car(v)    (list_car(v))
#define cdr(v)    (list_cdr(v))
#define caar(v)   (car(car(v)))
#define cadr(v)   (car(cdr(v)))
#define cdar(v)   (cdr(car(v)))
#define cddr(v)   (cdr(cdr(v)))
#define caaar(v)  (car(car(car(v))))
#define caadr(v)  (car(car(cdr(v))))
#define cadar(v)  (car(cdr(car(v))))
#define caddr(v)  (car(cdr(cdr(v))))
#define cdaar(v)  (cdr(car(car(v))))
#define cdadr(v)  (cdr(car(cdr(v))))
#define cddar(v)  (cdr(cdr(car(v))))
#define cdddr(v)  (cdr(cdr(cdr(v))))
#define caaaar(v) car(car(car(car(v))))
#define caaadr(v) car(car(car(cdr(v))))
#define caadar(v) car(car(cdr(car(v))))
#define caaddr(v) car(car(cdr(cdr(v))))
#define cadaar(v) car(cdr(car(car(v))))
#define cadadr(v) car(cdr(car(cdr(v))))
#define caddar(v) car(cdr(cdr(car(v))))
#define cadddr(v) car(cdr(cdr(cdr(v))))
#define cdaaar(v) cdr(car(car(car(v))))
#define cdaadr(v) cdr(car(car(cdr(v))))
#define cdadar(v) cdr(car(cdr(car(v))))
#define cdaddr(v) cdr(car(cdr(cdr(v))))
#define cddaar(v) cdr(cdr(car(car(v))))
#define cddadr(v) cdr(cdr(car(cdr(v))))
#define cdddar(v) cdr(cdr(cdr(car(v))))
#define cddddr(v) cdr(cdr(cdr(cdr(v))))

#endif
