#ifndef SQUEAKY_LVAL_H_INCLUDED
#define SQUEAKY_LVAL_H_INCLUDED

struct lenv;
struct lval;
typedef struct lval* (*lbuiltin)(struct lenv*, struct lval*);

enum lval_type {
    LVAL_TYPE_UNDEFINED = 0,
    LVAL_TYPE_ERROR,
    LVAL_TYPE_NUMBER,
    LVAL_TYPE_SYMBOL,
    LVAL_TYPE_FUNC,
    LVAL_TYPE_SEXPR,
    LVAL_TYPE_QEXPR,
};

struct lval {
    int type;

    char* error;
    long number;
    char* symbol;
    lbuiltin func;

    long cell_count;
    struct lval** cell;
};

struct lval* lval_make_number(long number);
struct lval* lval_make_error(const char* fmt, ...);
struct lval* lval_make_symbol(const char* symbol);
struct lval* lval_make_func(lbuiltin func);
struct lval* lval_make_sexpr(void);
struct lval* lval_make_qexpr(void);

struct lval* lval_copy(const struct lval* val);
void lval_free(struct lval* val);

const char* lval_type_name(int lval_type);
void lval_print(const struct lval* val);
void lval_println(const struct lval* val);

// operations on lists
struct lval* lval_list_append(struct lval* list, struct lval* val);
struct lval* lval_list_pop(struct lval* list, long i);
struct lval* lval_list_take(struct lval* list, long i);
struct lval* lval_list_join(struct lval* list, struct lval* extras);

#endif
