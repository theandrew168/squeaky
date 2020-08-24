#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "value.h"
#include "vm.h"

enum {
    GC_UNMARKED = 0,
    GC_MARKED,
};

static void
value_free(struct value* value)
{
    assert(value != NULL);

    switch (value->type) {
        case VALUE_STRING:
            free(value->as.string);
            break;
        case VALUE_SYMBOL:
            free(value->as.symbol);
            break;
        case VALUE_INPUT_PORT:
            if (value->as.port == stdin) break;
            fclose(value->as.port);
            break;
        case VALUE_OUTPUT_PORT:
            if (value->as.port == stdout) break;
            fclose(value->as.port);
            break;
        case VALUE_WINDOW:
            SDL_DestroyRenderer(value->as.window.renderer);
            SDL_DestroyWindow(value->as.window.window);
            break;
        case VALUE_EVENT:
            free(value->as.event);
            break;
        default:
            break;
    }
}

void
vm_init(struct vm* vm)
{
    assert(vm != NULL);

    vm->capacity = 1024 * 1024;
    vm->heap = calloc(vm->capacity, sizeof(struct value));

    // init the allocation free list
    vm->free = &vm->heap[0];
    for (long i = 0; i < vm->capacity - 1; i++) {
        vm->heap[i].next = &vm->heap[i + 1];
    }
    vm->heap[vm->capacity - 1].next = NULL;
}

void
vm_free(struct vm* vm)
{
    assert(vm != NULL);

    vm_gc(vm, NULL);
    free(vm->heap);

    vm->capacity = 0;
    vm->heap = NULL;
    vm->free = NULL;
}

static void
gc_mark(struct vm* vm, struct value* root)
{
    if (root == NULL) return;
    if (root->gc_mark == GC_MARKED) return;

    // mark everything reachable from a given root node
    root->gc_mark = GC_MARKED;

    // recursively mark the contents of a lambda
    if (value_is_lambda(root)) {
        gc_mark(vm, root->as.lambda.params);
        gc_mark(vm, root->as.lambda.body);
        gc_mark(vm, root->as.lambda.env);
    }

    // recursively mark pairs / lists
    if (value_is_pair(root)) {
        gc_mark(vm, root->as.pair.car);
        gc_mark(vm, root->as.pair.cdr);
    }
}

static void
gc_sweep(struct vm* vm)
{
    // sweep anything that isn't marked
    for (long i = 0; i < vm->capacity; i++) {
        if (vm->heap[i].gc_mark == GC_UNMARKED) {
            // free the value's dynamic contents
            value_free(&vm->heap[i]);
            vm->heap[i].type = VALUE_UNDEFINED;

            // put freed values back into the free list
            vm->heap[i].next = vm->free;
            vm->free = &vm->heap[i];
        }

        // every value goes back to unmarked after GC
        vm->heap[i].gc_mark = GC_UNMARKED;
    }
}

void
vm_gc(struct vm* vm, struct value* root)
{
    assert(vm != NULL);

    gc_mark(vm, root);
    gc_sweep(vm);
}

static struct value*
next_available_value(struct vm* vm)
{
    struct value* value = vm->free;
    if (value == NULL) {
        fprintf(stderr, "vm: out of memory\n");
        exit(EXIT_FAILURE);
    }

    // pull the free value out of the free list
    vm->free = value->next;
    value->next = NULL;
    return value;
}

struct value*
vm_make_empty_list(struct vm* vm)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_EMPTY_LIST;
    return value;
}

struct value*
vm_make_boolean(struct vm* vm, bool boolean)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_BOOLEAN;
    value->as.boolean = boolean;
    return value;
}

struct value*
vm_make_character(struct vm* vm, int character)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_CHARACTER;
    value->as.character = character;
    return value;
}

struct value*
vm_make_number(struct vm* vm, long number)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_NUMBER;
    value->as.number = number;
    return value;
}

struct value*
vm_make_string(struct vm* vm, const char* string)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_STRING;
    value->as.string = malloc(strlen(string) + 1);
    strcpy(value->as.string, string);
    return value;
}

struct value*
vm_make_symbol(struct vm* vm, const char* symbol)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_SYMBOL;
    value->as.symbol = malloc(strlen(symbol) + 1);
    strcpy(value->as.symbol, symbol);
    return value;
}

struct value*
vm_make_pair(struct vm* vm, struct value* car, struct value* cdr)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_PAIR;
    value->as.pair.car = car;
    value->as.pair.cdr = cdr;
    return value;
}

struct value*
vm_make_builtin(struct vm* vm, builtin_func builtin)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_BUILTIN;
    value->as.builtin = builtin;
    return value;
}

struct value*
vm_make_lambda(struct vm* vm, struct value* params, struct value* body, struct value* env)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_LAMBDA;
    value->as.lambda.params = params;
    value->as.lambda.body = body;
    value->as.lambda.env = env;
    return value;
}

struct value*
vm_make_input_port(struct vm* vm, FILE* port)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_INPUT_PORT;
    value->as.port = port;
    return value;
}

struct value*
vm_make_output_port(struct vm* vm, FILE* port)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_OUTPUT_PORT;
    value->as.port = port;
    return value;
}

struct value*
vm_make_window(struct vm* vm, const char* title, long width, long height)
{
    assert(vm != NULL);

    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        0);
    if (window == NULL) {
        fprintf(stderr, "failed to create SDL2 window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "failed to create SDL2 renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    struct value* value = next_available_value(vm);
    value->type = VALUE_WINDOW;
    value->as.window.window = window;
    value->as.window.renderer = renderer;
    return value;
}

struct value*
vm_make_event(struct vm* vm, SDL_Event* event)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_EVENT;
    value->as.event = event;
    return value;
}

struct value*
vm_make_eof(struct vm* vm)
{
    assert(vm != NULL);

    struct value* value = next_available_value(vm);
    value->type = VALUE_EOF;
    return value;
}
