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
typedef struct value* (*builtin_func)(struct value* args);

// TODO: consider a fancier, compressed, tagged uint64_t approach?
// would be 8 bytes per value instead of 32 (thats big!)
// 1 bit for GC marking, 4 bits for tagging, 59 bits for everything else (ints, floats, ptrs)
struct value {
    int type;  // this int will pad to 8 bytes on a 64-bit system
    union {
        bool boolean;
        int character;  // "int" for future-proofing UTF-8 support
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
        FILE* port;  // used for both VALUE_INPUT_PORT and VALUE_OUTPUT_PORT
        struct {
            SDL_Window* window;
            SDL_Renderer* renderer;
        } window;
        SDL_Event* event;
    } as;
};

// making the EMPTY_LIST be NULL has both pros and cons:
// a lot of list iteration code becomes simpler but the
// need arises for extra NULL checks in other parts of the code
// TODO: would it be better to just make a VALUE_EMPTY_LIST?
#define EMPTY_LIST NULL

// dynamic type checks
bool value_is_empty_list(const struct value* exp);
bool value_is_boolean(const struct value* exp);
bool value_is_true(const struct value* exp);
bool value_is_false(const struct value* exp);
bool value_is_character(const struct value* exp);
bool value_is_number(const struct value* exp);
bool value_is_string(const struct value* exp);
bool value_is_symbol(const struct value* exp);
bool value_is_pair(const struct value* exp);
bool value_is_builtin(const struct value* exp);
bool value_is_lambda(const struct value* exp);
bool value_is_procedure(const struct value* exp);
bool value_is_input_port(const struct value* exp);
bool value_is_output_port(const struct value* exp);
bool value_is_window(const struct value* exp);
bool value_is_event(const struct value* exp);
bool value_is_eof(const struct value* exp);

// constructors
struct value* value_make_empty_list(void);
struct value* value_make_boolean(bool boolean);
struct value* value_make_character(int character);
struct value* value_make_number(long number);
struct value* value_make_string(const char* string);
struct value* value_make_stringn(const char* string, long length);
struct value* value_make_symbol(const char* symbol);
struct value* value_make_symboln(const char* symbol, long length);
struct value* value_make_pair(struct value* car, struct value* cdr);
struct value* value_make_builtin(builtin_func builtin);
struct value* value_make_lambda(struct value* params, struct value* body, struct value* env);
struct value* value_make_input_port(FILE* port);
struct value* value_make_output_port(FILE* port);
struct value* value_make_window(const char* title, long width, long height);
struct value* value_make_event(SDL_Event* event);
struct value* value_make_eof(void);

// printing
void value_print(FILE* fp, const struct value* value);
void value_println(FILE* fp, const struct value* value);
const char* value_type_name(int type);

// comparison
bool value_is_eq(const struct value* a, const struct value* b);
bool value_is_eqv(const struct value* a, const struct value* b);
bool value_is_equal(const struct value* a, const struct value* b);

#define cons(a,b) (value_make_pair((a), (b)))

#endif
