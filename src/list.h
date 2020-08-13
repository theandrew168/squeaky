#ifndef SQUEAKY_LIST_H_INCLUDED
#define SQUEAKY_LIST_H_INCLUDED

#include <stdarg.h>
#include <stdlib.h>

#include "list.h"

#define list_is_null(l) ((l) == EMPTY_LIST)

struct value* list_make(long count, struct value* value, ...);
long list_length(const struct value* list);
struct value* list_nth(const struct value* list, long n);

struct value* list_car(const struct value* list);
struct value* list_cdr(const struct value* list);

#define ASSERTF(cond, fmt, ...)           \
  if (!(cond)) {                          \
    fprintf(stderr, fmt, ##__VA_ARGS__);  \
    exit(EXIT_FAILURE);                   \
  }

#define ASSERT_ARITY(func, args, count)                                   \
  ASSERTF(list_length(args) == count,                                     \
    "function '%s' passed incorrect number of args: want %d, got %ld\n",  \
    func, count, list_length(args))

#define ASSERT_ARITY_OR(func, args, a, b)                                       \
  ASSERTF(list_length(args) == (a) || list_length(args) == (b),                 \
    "function '%s' passed incorrect number of args: want %d or %d, got %ld\n",  \
    func, a, b, list_length(args))

#define ASSERT_ARITY_LTE(func, args, count)                            \
  ASSERTF(list_length(args) <= count,                                  \
    "function '%s' passed too many args: want at most %d, got %ld\n",  \
    func, count, list_length(args))

#define ASSERT_ARITY_GTE(func, args, count)                            \
  ASSERTF(list_length(args) >= count,                                  \
    "function '%s' passed too few args: want at least %d, got %ld\n",  \
    func, count, list_length(args))

#define ASSERT_TYPE(func, args, index, want)                              \
  ASSERTF(list_nth(args, index)->type == want,                            \
    "function '%s' passed incorrect type for arg %i: want %s, got %s\n",  \
    func, index,                                                          \
    value_type_name(want),                                                \
    value_type_name(list_nth(args, index)->type))

#define ASSERT_TYPE_ALL(func, args, want)        \
  for (int i = 0; i < list_length(args); i++) {  \
    ASSERT_TYPE(func, args, i, want)             \
  }

#define car(v)    (list_car(v))
#define cdr(v)    (list_cdr(v))
#define caar(v)   (car(car(v)))
#define cadr(v)   (car(cdr(v)))
#define cdar(v)   (cdr(car(v)))
#define cddr(v)   (cdr(cdr(v)))
#define caaar(v)  (car(car(car(v))))
#define caadr(v)  (car(car(cdr(v))))
#define cadar(v)  (car(cdr(car(v))))
#define caddr(v)  (car(cdr(cdr(v))))
#define cdaar(v)  (cdr(car(car(v))))
#define cdadr(v)  (cdr(car(cdr(v))))
#define cddar(v)  (cdr(cdr(car(v))))
#define cdddr(v)  (cdr(cdr(cdr(v))))
#define caaaar(v) car(car(car(car(v))))
#define caaadr(v) car(car(car(cdr(v))))
#define caadar(v) car(car(cdr(car(v))))
#define caaddr(v) car(car(cdr(cdr(v))))
#define cadaar(v) car(cdr(car(car(v))))
#define cadadr(v) car(cdr(car(cdr(v))))
#define caddar(v) car(cdr(cdr(car(v))))
#define cadddr(v) car(cdr(cdr(cdr(v))))
#define cdaaar(v) cdr(car(car(car(v))))
#define cdaadr(v) cdr(car(car(cdr(v))))
#define cdadar(v) cdr(car(cdr(car(v))))
#define cdaddr(v) cdr(car(cdr(cdr(v))))
#define cddaar(v) cdr(cdr(car(car(v))))
#define cddadr(v) cdr(cdr(car(cdr(v))))
#define cdddar(v) cdr(cdr(cdr(car(v))))
#define cddddr(v) cdr(cdr(cdr(cdr(v))))

#endif
