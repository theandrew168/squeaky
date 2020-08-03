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
#define value_is_error(v)   ((v)->type == VALUE_ERROR)
bool value_is_true(const struct value* exp);
bool value_is_false(const struct value* exp);
bool value_is_procedure(const struct value* exp);

struct value* value_make_boolean(bool boolean);
struct value* value_make_number(long number);
struct value* value_make_string(const char* string, long length);
struct value* value_make_symbol(const char* symbol, long length);
struct value* value_make_pair(struct value* car, struct value* cdr);
struct value* value_make_builtin(builtin_func builtin);
struct value* value_make_lambda(struct value* params, struct value* body, struct value* env);
struct value* value_make_window(const char* title, long width, long height);
struct value* value_make_error(const char* error);
void value_free(struct value* value);

struct value* value_read(const char* str, long* consumed);
void value_write(const struct value* value);

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

#endif
