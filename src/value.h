#ifndef SQUEAKY_VALUE_H_INCLUDED
#define SQUEAKY_VALUE_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

enum value_type {
    VALUE_UNDEFINED = 0,
    VALUE_EMPTY_LIST,
    VALUE_BOOLEAN,
    VALUE_CHARACTER,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_SYMBOL,
    VALUE_VECTOR,
    VALUE_PAIR,
    VALUE_BUILTIN,
    VALUE_LAMBDA,
    VALUE_INPUT_PORT,
    VALUE_OUTPUT_PORT,
    VALUE_WINDOW,
    VALUE_EVENT,
    VALUE_EOF,
};

struct value;
struct vm;
typedef struct value* (*builtin_func)(struct vm* vm, struct value* args);

struct value {
    int type;
    int gc_mark;
// TODO: SLL for free list?
//    struct value* next;
    union {
        bool boolean;
        int character;  // "int" for future-proofing UTF-8 support
        long number;
        char* string;
        char* symbol;
        struct {
            long count;
            long capacity;
            struct value** array;
        } vector;
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
        FILE* port;  // used for both VALUE_INPUT_PORT and VALUE_OUTPUT_PORT
        struct {
            SDL_Window* window;
            SDL_Renderer* renderer;
        } window;
        SDL_Event* event;
    } as;
};

// singular type checks
#define value_is_empty_list(value)  ((value)->type == VALUE_EMPTY_LIST)
#define value_is_boolean(value)     ((value)->type == VALUE_BOOLEAN)
#define value_is_character(value)   ((value)->type == VALUE_CHARACTER)
#define value_is_number(value)      ((value)->type == VALUE_NUMBER)
#define value_is_string(value)      ((value)->type == VALUE_STRING)
#define value_is_symbol(value)      ((value)->type == VALUE_SYMBOL)
#define value_is_pair(value)        ((value)->type == VALUE_PAIR)
#define value_is_builtin(value)     ((value)->type == VALUE_BUILTIN)
#define value_is_lambda(value)      ((value)->type == VALUE_LAMBDA)
#define value_is_input_port(value)  ((value)->type == VALUE_INPUT_PORT)
#define value_is_output_port(value) ((value)->type == VALUE_OUTPUT_PORT)
#define value_is_window(value)      ((value)->type == VALUE_WINDOW)
#define value_is_event(value)       ((value)->type == VALUE_EVENT)
#define value_is_eof(value)         ((value)->type == VALUE_EOF)

// composite type checks (would be unsafe as macros)
bool value_is_true(const struct value* value);
bool value_is_false(const struct value* value);
bool value_is_procedure(const struct value* value);

// printing
void value_print(FILE* fp, const struct value* value);
void value_println(FILE* fp, const struct value* value);
const char* value_type_name(int type);

// comparison
bool value_is_eq(const struct value* a, const struct value* b);
bool value_is_eqv(const struct value* a, const struct value* b);
bool value_is_equal(const struct value* a, const struct value* b);

#endif
