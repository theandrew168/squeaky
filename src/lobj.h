#ifndef SQUEAKY_LOBJ_H_INCLUDED
#define SQUEAKY_LOBJ_H_INCLUDED

struct lenv;
struct lobj;
typedef struct lobj* (*lbuiltin)(struct lenv*, struct lobj*);

enum lobj_type {
    LOBJ_TYPE_UNDEFINED = 0,
    LOBJ_TYPE_ERROR,
    LOBJ_TYPE_NUMBER,
    LOBJ_TYPE_SYMBOL,
    LOBJ_TYPE_FUNC,
    LOBJ_TYPE_SEXPR,
    LOBJ_TYPE_QEXPR,
};

struct lobj {
    int type;

    char* error;
    long number;
    char* symbol;
    lbuiltin func;

    long cell_count;
    struct lobj** cell;
};

struct lobj* lobj_make_number(long number);
struct lobj* lobj_make_error(const char* fmt, ...);
struct lobj* lobj_make_symbol(const char* symbol);
struct lobj* lobj_make_func(lbuiltin func);
struct lobj* lobj_make_sexpr(void);
struct lobj* lobj_make_qexpr(void);

struct lobj* lobj_copy(const struct lobj* obj);
void lobj_free(struct lobj* obj);

void lobj_print(const struct lobj* obj);
void lobj_println(const struct lobj* obj);

// operations on lists
struct lobj* lobj_list_append(struct lobj* list, struct lobj* obj);
struct lobj* lobj_list_pop(struct lobj* list, long i);
struct lobj* lobj_list_take(struct lobj* list, long i);
struct lobj* lobj_list_join(struct lobj* list, struct lobj* extras);

#endif
