#ifndef SQUEAKY_LVAL_STRING_H_INCLUDED
#define SQUEAKY_LVAL_STRING_H_INCLUDED

#include <stdbool.h>

struct lval;

bool lval_string_init(struct lval* val, const char* string);
void lval_string_free(struct lval* val);
void lval_string_copy(const struct lval* val, struct lval* copy);

void lval_string_print(const struct lval* val);
bool lval_string_equal(const struct lval* a, const struct lval* b);

#endif
