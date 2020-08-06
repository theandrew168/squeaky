#ifndef SQUEAKY_VALUE_H_INCLUDED
#define SQUEAKY_VALUE_H_INCLUDED

#include <stdbool.h>
#include <stdlib.h>

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

#define EMPTY_LIST NULL

bool value_is_boolean(const struct value* exp);
bool value_is_number(const struct value* exp);
bool value_is_string(const struct value* exp);
bool value_is_symbol(const struct value* exp);
bool value_is_pair(const struct value* exp);
bool value_is_builtin(const struct value* exp);
bool value_is_lambda(const struct value* exp);
bool value_is_window(const struct value* exp);
bool value_is_event(const struct value* exp);
bool value_is_error(const struct value* exp);
bool value_is_procedure(const struct value* exp);

bool value_is_true(const struct value* exp);
bool value_is_false(const struct value* exp);

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

// ref count functions
void value_ref_inc(struct value* value);
void value_ref_dec(struct value* value);

// extra goodies
bool value_equal(const struct value* a, const struct value* b);

#define cons(a,b) (value_make_pair((a), (b)))

#endif
