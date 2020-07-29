#ifndef SQUEAKY_VALUE_H_INCLUDED
#define SQUEAKY_VALUE_H_INCLUDED

#include <stdbool.h>

enum value_type {
    VALUE_UNDEFINED = 0,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_SYMBOL,
    VALUE_PAIR,
    VALUE_BUILTIN,
    VALUE_LAMBDA,
};

struct value;
struct env;
typedef struct value* (*builtin_func)(struct value* value, struct env* env);

// 16 bytes on a 32-bit system?
// 32 bytes on a 64-bit system?
// assuming the int will pad to 8 bytes and
//   lambda is the biggest value (3 * 8 = 24)
struct value {
    int type;
    union {
        double number;
        char* string;
        char* symbol;
        struct {
            struct value* car;
            struct value* cdr;
        } pair;
        builtin_func builtin;
        struct {
            struct value* params;
            struct value* body;
            struct env* env;
        } lambda;
    } as;
};

#define CONS(a,b) (value_make_pair((a), (b)))

#define CAR(v)    ((v)->as.pair.car)
#define CDR(v)    ((v)->as.pair.cdr)
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
#define CAAAAR(v) (CAR(CAR(CAR(CAR(v)))))
#define CAAADR(v) (CAR(CAR(CAR(CDR(v)))))
#define CAADAR(v) (CAR(CAR(CDR(CAR(v)))))
#define CAADDR(v) (CAR(CAR(CDR(CDR(v)))))
#define CADAAR(v) (CAR(CDR(CAR(CAR(v)))))
#define CADADR(v) (CAR(CDR(CAR(CDR(v)))))
#define CADDAR(v) (CAR(CDR(CDR(CAR(v)))))
#define CADDDR(v) (CAR(CDR(CDR(CDR(v)))))
#define CDAAAR(v) (CDR(CAR(CAR(CAR(v)))))
#define CDAADR(v) (CDR(CAR(CAR(CDR(v)))))
#define CDADAR(v) (CDR(CAR(CDR(CAR(v)))))
#define CDADDR(v) (CDR(CAR(CDR(CDR(v)))))
#define CDDAAR(v) (CDR(CDR(CAR(CAR(v)))))
#define CDDADR(v) (CDR(CDR(CAR(CDR(v)))))
#define CDDDAR(v) (CDR(CDR(CDR(CAR(v)))))
#define CDDDDR(v) (CDR(CDR(CDR(CDR(v)))))

struct value* value_make_number(double number);
struct value* value_make_string(const char* string, long length);
struct value* value_make_symbol(const char* symbol, long length);
struct value* value_make_pair(struct value* car, struct value* cdr);
void value_free(struct value* value);

bool value_is_self_evaluating(struct value* value);
bool value_is_variable(struct value* value);
bool value_is_quoted(struct value* value);
bool value_is_assignment(struct value* value);
bool value_is_definition(struct value* value);
bool value_is_if(struct value* value);
bool value_is_application(struct value* value);
bool value_is_null(struct value* value);

void value_print(const struct value* value);
void value_println(const struct value* value);

#endif
