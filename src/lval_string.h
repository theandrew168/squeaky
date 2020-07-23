#ifndef SQUEAKY_LVAL_STRING_H_INCLUDED
#define SQUEAKY_LVAL_STRING_H_INCLUDED

#include <stdbool.h>

struct lval_string {
    int type;
    char* string;
};

bool lval_string_init(struct lval_string* val, const char* string);
void lval_string_free(struct lval_string* val);
void lval_string_copy(const struct lval_string* val, struct lval_string* copy);

void lval_string_print(const struct lval_string* val);
bool lval_string_equal(const struct lval_string* a, const struct lval_string* b);

#endif
