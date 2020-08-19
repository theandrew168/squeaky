#ifndef SQUEAKY_VM_H_INCLUDED
#define SQUEAKY_VM_H_INCLUDED

#include <stdint.h>

// NaN Tagging / Boxing based on Crafting Interpreters:
// https://craftinginterpreters.com/optimization.html#nan-boxing
// https://github.com/munificent/craftinginterpreters/blob/master/c/value.h
// https://github.com/munificent/craftinginterpreters/blob/master/c/object.h

typedef uint64_t Value;

// 13 bits are used by QNAN
#define QNAN           ((uint64_t)0x7ffc000000000000)

// 3 bits (63, 49, 48) of NaN tagging allows for 8 different value types
#define TAG_UNDEFINED  ((uint64_t)0x7ffc000000000000)
#define TAG_EMPTY_LIST ((uint64_t)0x7ffd000000000000)
#define TAG_EOF        ((uint64_t)0x7ffe000000000000)
#define TAG_PAIR       ((uint64_t)0x7fff000000000000)
#define TAG_BOOLEAN    ((uint64_t)0xfffc000000000000)
#define TAG_CHARACTER  ((uint64_t)0xfffd000000000000)
#define TAG_BUILTIN    ((uint64_t)0xfffe000000000000)
#define TAG_OBJECT     ((uint64_t)0xffff000000000000)

// 48 remaining bits for value data: pairs, booleans, characters, and pointers
#define VALUE_DATA     ((uint64_t)0x0000ffffffffffff)

// check if a value is a certain type by masking and comparing its tag
// NOTE: the number type doesn't need a tag because it simply won't be a QNAN
#define IS_NUMBER(v)     (((v) & QNAN) != QNAN)
#define IS_UNDEFINED(v)  (((v) & TAG_UNDEFINED) == TAG_UNDEFINED)
#define IS_EMPTY_LIST(v) (((v) & TAG_EMPTY_LIST) == TAG_EMPTY_LIST)
#define IS_EOF(v)        (((v) & TAG_EOF) == TAG_EOF)
#define IS_PAIR(v)       (((v) & TAG_PAIR) == TAG_PAIR)
#define IS_BOOLEAN(v)    (((v) & TAG_BOOLEAN) == TAG_BOOLEAN)
#define IS_CHARACTER(v)  (((v) & TAG_CHARACTER) == TAG_CHARACTER)
#define IS_BUILTIN(v)    (((v) & TAG_BUILTIN) == TAG_BUILTIN)
#define IS_OBJECT(v)     (((v) & TAG_OBJECT) == TAG_OBJECT)

enum object_type {
    OBJECT_UNDEFINED = 0,
    OBJECT_STRING,
    OBJECT_SYMBOL,
//    OBJECT_VECTOR,
//    OBJECT_VECTOR_U8,
//    OBJECT_VECTOR_F32,
    OBJECT_LAMBDA,
    OBJECT_INPUT_PORT,
    OBJECT_OUTPUT_PORT,
    OBJECT_WINDOW,
    OBJECT_EVENT,
};

struct object {
    int type;
    int gc_mark;
    struct object* next;
};

struct object_string {
    struct object object;
    char* string;
};

struct object_symbol {
    struct object object;
    char* symbol;
};

struct object_lambda {
    struct object object;
    Value params;
    Value body;
    Value env;
};

struct object_input_port {
    struct object object;
    FILE* stream;
};

struct object_output_port {
    struct object object;
    FILE* stream;
};

struct object_window {
    struct object object;
    SDL_Window* window;
    SDL_Renderer* renderer;
};

struct object_event {
    struct object object;
    SDL_Event* event;
};

struct vm {
    long count;
    long capacity;
    Value* heap;    
};

void vm_init(struct vm* vm);
void vm_free(struct vm* vm);

#endif
