#ifndef SQUEAKY_LIST_H_INCLUDED
#define SQUEAKY_LIST_H_INCLUDED

#include <stdarg.h>
#include <stdlib.h>

#include "value.h"

//struct value* list_make(long count, struct value* value, ...);
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

#define CAR(v)    (list_car(v))
#define CDR(v)    (list_cdr(v))
#define CAAR(v)   (CAR(CAR(v)))
#define CADR(v)   (CAR(CDR(v)))
#define CDAR(v)   (CDR(CAR(v)))
#define CDDR(v)   (CDR(CDR(v)))
#define CAAAR(v)  (CAR(CAR(CAR(v))))
#define CAADR(v)  (CAR(CAR(CDR(v))))
#define CADAR(v)  (CAR(CDR(CAR(v))))
#define CADDR(v)  (CAR(CDR(CDR(v))))
#define CDAAR(v)  (CDR(CAR(CAR(v))))
#define CDADR(v)  (CDR(CAR(CDR(v))))
#define CDDAR(v)  (CDR(CDR(CAR(v))))
#define CDDDR(v)  (CDR(CDR(CDR(v))))
#define CAAAAR(v) CAR(CAR(CAR(CAR(v))))
#define CAAADR(v) CAR(CAR(CAR(CDR(v))))
#define CAADAR(v) CAR(CAR(CDR(CAR(v))))
#define CAADDR(v) CAR(CAR(CDR(CDR(v))))
#define CADAAR(v) CAR(CDR(CAR(CAR(v))))
#define CADADR(v) CAR(CDR(CAR(CDR(v))))
#define CADDAR(v) CAR(CDR(CDR(CAR(v))))
#define CADDDR(v) CAR(CDR(CDR(CDR(v))))
#define CDAAAR(v) CDR(CAR(CAR(CAR(v))))
#define CDAADR(v) CDR(CAR(CAR(CDR(v))))
#define CDADAR(v) CDR(CAR(CDR(CAR(v))))
#define CDADDR(v) CDR(CAR(CDR(CDR(v))))
#define CDDAAR(v) CDR(CDR(CAR(CAR(v))))
#define CDDADR(v) CDR(CDR(CAR(CDR(v))))
#define CDDDAR(v) CDR(CDR(CDR(CAR(v))))
#define CDDDDR(v) CDR(CDR(CDR(CDR(v))))

#endif
