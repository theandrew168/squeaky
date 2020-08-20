#ifndef SQUEAKY_VM_H_INCLUDED
#define SQUEAKY_VM_H_INCLUDED

#include "value.h"

// NaN Tagging / Boxing based on Crafting Interpreters:
// https://craftinginterpreters.com/optimization.html#nan-boxing
// https://github.com/munificent/craftinginterpreters/blob/master/c/value.h
// https://github.com/munificent/craftinginterpreters/blob/master/c/object.h

//typedef uint64_t Value;
//
//// 13 bits are used by QNAN
//#define QNAN           ((uint64_t)0x7ffc000000000000)
//
//// 3 bits (63, 49, 48) of NaN tagging allows for 8 different value types
//#define TAG_UNDEFINED  ((uint64_t)0x7ffc000000000000)
//#define TAG_EMPTY_LIST ((uint64_t)0x7ffd000000000000)
//#define TAG_EOF        ((uint64_t)0x7ffe000000000000)
//#define TAG_PAIR       ((uint64_t)0x7fff000000000000)
//#define TAG_BOOLEAN    ((uint64_t)0xfffc000000000000)
//#define TAG_CHARACTER  ((uint64_t)0xfffd000000000000)
//#define TAG_BUILTIN    ((uint64_t)0xfffe000000000000)
//#define TAG_OBJECT     ((uint64_t)0xffff000000000000)
//
//// 48 remaining bits for value data: pairs, booleans, characters, and pointers
//#define VALUE_DATA     ((uint64_t)0x0000ffffffffffff)
//
//// check if a value is a certain type by masking and comparing its tag
//// NOTE: the number type doesn't need a tag because it simply won't be a QNAN
//#define IS_NUMBER(v)     (((v) & QNAN) != QNAN)
//#define IS_UNDEFINED(v)  (((v) & TAG_UNDEFINED) == TAG_UNDEFINED)
//#define IS_EMPTY_LIST(v) (((v) & TAG_EMPTY_LIST) == TAG_EMPTY_LIST)
//#define IS_EOF(v)        (((v) & TAG_EOF) == TAG_EOF)
//#define IS_PAIR(v)       (((v) & TAG_PAIR) == TAG_PAIR)
//#define IS_BOOLEAN(v)    (((v) & TAG_BOOLEAN) == TAG_BOOLEAN)
//#define IS_CHARACTER(v)  (((v) & TAG_CHARACTER) == TAG_CHARACTER)
//#define IS_BUILTIN(v)    (((v) & TAG_BUILTIN) == TAG_BUILTIN)
//#define IS_OBJECT(v)     (((v) & TAG_OBJECT) == TAG_OBJECT)
//
//enum object_type {
//    OBJECT_UNDEFINED = 0,
//    OBJECT_STRING,
//    OBJECT_SYMBOL,
////    OBJECT_VECTOR,
////    OBJECT_VECTOR_U8,
////    OBJECT_VECTOR_F32,
//    OBJECT_LAMBDA,
//    OBJECT_INPUT_PORT,
//    OBJECT_OUTPUT_PORT,
//    OBJECT_WINDOW,
//    OBJECT_EVENT,
//};
//
//struct object {
//    int type;
//    int gc_mark;
//    struct object* next;
//    union {
//        char* string;
//        char* symbol;
//        struct {
//            Value params;
//            Value body;
//            Value env;
//        } lambda;
//        FILE* stream;
//        struct {
//            SDL_Window* window;
//            SDL_Renderer* renderer;
//        } window;
//        SDL_Event* event;
//    } as;
//};

struct vm {
    long capacity;
    struct value* heap;    
    struct value* free;
};

void vm_init(struct vm* vm);
void vm_free(struct vm* vm);
void vm_gc(struct vm* vm, struct value* root);

struct value* vm_make_empty_list(struct vm* vm);
struct value* vm_make_boolean(struct vm* vm, bool boolean);
struct value* vm_make_character(struct vm* vm, int character);
struct value* vm_make_number(struct vm* vm, long number);
struct value* vm_make_string(struct vm* vm, const char* string);
struct value* vm_make_symbol(struct vm* vm, const char* symbol);
struct value* vm_make_pair(struct vm* vm, struct value* car, struct value* cdr);
struct value* vm_make_builtin(struct vm* vm, builtin_func builtin);
struct value* vm_make_lambda(struct vm* vm, struct value* params, struct value* body, struct value* env);
struct value* vm_make_input_port(struct vm* vm, FILE* port);
struct value* vm_make_output_port(struct vm* vm, FILE* port);
struct value* vm_make_window(struct vm* vm, const char* title, long width, long height);
struct value* vm_make_event(struct vm* vm, SDL_Event* event);
struct value* vm_make_eof(struct vm* vm);

#endif
