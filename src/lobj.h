#ifndef SQUEAKY_LOBJ_H_INCLUDED
#define SQUEAKY_LOBJ_H_INCLUDED

enum lobj_type {
    LOBJ_TYPE_UNDEFINED = 0,
    LOBJ_TYPE_ERROR,
    LOBJ_TYPE_NUMBER,
    LOBJ_TYPE_SYMBOL,
    LOBJ_TYPE_SEXPR,
};

struct lobj {
    int type;
    char* error;
    long number;
    char* symbol;
    long cell_count;
    struct lobj** cell;
};

struct lobj* lobj_make_number(long number);
struct lobj* lobj_make_error(const char* error);
struct lobj* lobj_make_symbol(const char* symbol);
struct lobj* lobj_make_sexpr(void);

void lobj_free(struct lobj* obj);
void lobj_print(const struct lobj* obj);
void lobj_println(const struct lobj* obj);

void lobj_sexpr_append(struct lobj* sexpr, struct lobj* obj);
struct lobj* lobj_sexpr_pop(struct lobj* sexpr, long i);
struct lobj* lobj_sexpr_take(struct lobj* sexpr, long i);

#endif
