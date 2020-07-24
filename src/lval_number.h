#ifndef SQUEAKY_LVAL_NUMBER_H_INCLUDED
#define SQUEAKY_LVAL_NUMBER_H_INCLUDED

#include <stdbool.h>

union lval;

// TODO: this can be expanded with GMP for big number funsies
struct lval_number {
    int type;
    long number;
};

#define AS_NUMBER(val) ((struct lval_number*)(val))
#define AS_CONST_NUMBER(val) ((const struct lval_number*)(val))

bool lval_number_init(union lval* val, long number);
void lval_number_free(union lval* val);
void lval_number_copy(const union lval* val, union lval* copy);

void lval_number_print(const union lval* val);
bool lval_number_equal(const union lval* a, const union lval* b);

#endif
