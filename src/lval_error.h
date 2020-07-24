#ifndef SQUEAKY_LVAL_ERROR_H_INCLUDED
#define SQUEAKY_LVAL_ERROR_H_INCLUDED

#include <stdarg.h>
#include <stdbool.h>

struct lval;

bool lval_error_init(struct lval* val, const char* fmt, va_list args);
void lval_error_free(struct lval* val);
void lval_error_copy(const struct lval* val, struct lval* copy);

void lval_error_print(const struct lval* val);
bool lval_error_equal(const struct lval* a, const struct lval* b);

#endif
