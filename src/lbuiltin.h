#ifndef SQUEAKY_LBUILTIN_H_INCLUDED
#define SQUEAKY_LBUILTIN_H_INCLUDED

#define LASSERT(args, cond, err)      \
    if (!(cond)) {                    \
        lval_free(args);              \
        return lval_make_error(err);  \
    }

#define LASSERTF(args, cond, fmt, ...)                          \
    if (!(cond)) {                                              \
        struct lval* err = lval_make_error(fmt, ##__VA_ARGS__);  \
        lval_free(args);                                        \
        return err;                                             \
    }

#define LASSERT_ARITY(func, args, num)          \
    LASSERTF(args, args->count == num,          \
        "function '%s' passed too many args: "  \
        "want %i, got %i",                      \
        func, num, args->count);

#define LASSERT_TYPE(func, args, index, lval_type)          \
    LASSERTF(args, args->list[index]->type == lval_type,    \
        "function '%s' passed incorrect type for arg %i: "  \
        "want %s, got %s",                                  \
        func, index, lval_type_name(lval_type),             \
        lval_type_name(args->list[index]->type));

#define LASSERT_NOT_EMPTY(func, args, index)             \
    LASSERTF(args, args->list[index]->count != 0,        \
        "function '%s' passed empty list for arg %i: ",  \
        func, index);

struct lenv;
struct lval;
typedef struct lval* (*lbuiltin)(struct lenv*, struct lval*);

#endif
