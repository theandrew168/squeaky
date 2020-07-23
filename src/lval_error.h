#ifndef SQUEAKY_LVAL_ERROR_H_INCLUDED
#define SQUEAKY_LVAL_ERROR_H_INCLUDED

#include <stdarg.h>
#include <stdbool.h>

struct lval_error {
    int type;
    char* error;
};

bool lval_error_init(struct lval_error* val, const char* fmt, ...);
void lval_error_free(struct lval_error* val);
void lval_error_copy(const struct lval_error* val, struct lval_error* copy);

void lval_error_print(const struct lval_error* val);
bool lval_error_equal(const struct lval_error* a, const struct lval_error* b);

#endif
