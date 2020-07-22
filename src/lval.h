#ifndef SQUEAKY_LVAL_H_INCLUDED
#define SQUEAKY_LVAL_H_INCLUDED

#include <stdbool.h>

#define LASSERT(args, cond, err)  \
    if (!(cond)) { lval_free(args); return lval_make_error(err); }

#define LASSERTF(args, cond, fmt, ...)                           \
    if (!(cond)) {                                               \
        struct lval* err = lval_make_error(fmt, ##__VA_ARGS__);  \
        lval_free(args);                                         \
        return err;                                              \
    }

#define LASSERT_ARITY(func, args, num)          \
    LASSERTF(args, args->cell_count == num,     \
        "function '%s' passed too many args: "  \
        "want %i, got %i",                      \
        func, num, args->cell_count);

#define LASSERT_TYPE(func, args, index, lval_type)          \
    LASSERTF(args, args->cell[index]->type == lval_type,    \
        "function '%s' passed incorrect type for arg %i: "  \
        "want %s, got %s",                                  \
        func, index, lval_type_name(lval_type), lval_type_name(args->cell[index]->type));

#define LASSERT_NOT_EMPTY(func, args, index)             \
    LASSERTF(args, args->cell[index]->cell_count != 0,   \
        "function '%s' passed empty list for arg %i: ",  \
        func, index);

struct lenv;
struct lval;
typedef struct lval* (*lbuiltin)(struct lenv*, struct lval*);

enum lval_type {
    LVAL_TYPE_UNDEFINED = 0,
    LVAL_TYPE_ERROR,
    LVAL_TYPE_NUMBER,
    LVAL_TYPE_SYMBOL,
    LVAL_TYPE_STRING,
    LVAL_TYPE_FUNC,
    LVAL_TYPE_SEXPR,
    LVAL_TYPE_QEXPR,
};

struct lval {
    int type;

    char* error;
    long number;
    char* symbol;
    char* string;

    lbuiltin builtin;
    struct lenv* env;
    struct lval* formals;
    struct lval* body;

    long cell_count;
    struct lval** cell;
};

// constructors for each type
struct lval* lval_make_number(long number);
struct lval* lval_make_error(const char* fmt, ...);
struct lval* lval_make_symbol(const char* symbol);
struct lval* lval_make_string(const char* string);
struct lval* lval_make_builtin(lbuiltin builtin);
struct lval* lval_make_lambda(struct lval* formals, struct lval* body);
struct lval* lval_make_sexpr(void);
struct lval* lval_make_qexpr(void);

// misc stuff
struct lval* lval_copy(const struct lval* val);
void lval_free(struct lval* val);
bool lval_eq(const struct lval* x, const struct lval* y);
const char* lval_type_name(int lval_type);

// printing
void lval_print(const struct lval* val);
void lval_println(const struct lval* val);

// operations on lists
struct lval* lval_list_append(struct lval* list, struct lval* val);
struct lval* lval_list_pop(struct lval* list, long i);
struct lval* lval_list_take(struct lval* list, long i);
struct lval* lval_list_join(struct lval* list, struct lval* extras);

struct lval* lval_read_expr(char* s, long* i, char end);

#endif
