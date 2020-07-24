#ifndef SQUEAKY_LVAL_LIST_H_INCLUDED
#define SQUEAKY_LVAL_LIST_H_INCLUDED

#include <stdbool.h>

struct lval;

bool lval_list_init(struct lval* val, int type);
void lval_list_free(struct lval* val);
void lval_list_copy(const struct lval* val, struct lval* copy);

void lval_list_print(const struct lval* val, char start, char end);
bool lval_list_equal(const struct lval* a, const struct lval* b);

struct lval* lval_list_append(struct lval* list, struct lval* val);
struct lval* lval_list_pop(struct lval* list, long i);
struct lval* lval_list_take(struct lval* list, long i);
struct lval* lval_list_join(struct lval* list, struct lval* extras);

#endif
