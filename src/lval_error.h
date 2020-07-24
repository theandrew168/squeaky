#ifndef SQUEAKY_LVAL_ERROR_H_INCLUDED
#define SQUEAKY_LVAL_ERROR_H_INCLUDED

#include <stdarg.h>
#include <stdbool.h>

union lval;

struct lval_error {
    int type;
    char* error;
};

#define AS_ERROR(val) ((struct lval_error*)(val))
#define AS_CONST_ERROR(val) ((const struct lval_error*)(val))

bool lval_error_init(union lval* val, const char* fmt, va_list args);
void lval_error_free(union lval* val);
void lval_error_copy(const union lval* val, union lval* copy);

void lval_error_print(const union lval* val);
bool lval_error_equal(const union lval* a, const union lval* b);

#endif
