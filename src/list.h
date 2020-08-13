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
