#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum value_type {
    VALUE_UNDEFINED = 0,
    VALUE_NUMBER,
    VALUE_SYMBOL,
    VALUE_PAIR,
    VALUE_BUILTIN,
    VALUE_LAMBDA,
};

struct value;
typedef struct value* (*builtin_func)(struct value* value, struct value* env);

struct value {
    int type;
    union {
        long number;
        char* symbol;
        struct {
            struct value* car;
            struct value* cdr;
        } pair;
        builtin_func builtin;
        struct {
            struct value* params;
            struct value* body;
            struct value* env;
        } lambda;
    } as;
};

struct value*
value_make_number(long number)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_NUMBER;
    value->as.number = number;
    return value;
}

struct value*
value_make_symbol(const char* symbol, long length)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_SYMBOL;
    value->as.symbol = malloc(length + 1);
    strncpy(value->as.symbol, symbol, length);
    value->as.symbol[length] = '\0';
    return value;
}

struct value*
value_make_pair(struct value* car, struct value* cdr)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_PAIR;
    value->as.pair.car = car;
    value->as.pair.cdr = cdr;
    return value;
}

struct value*
value_make_builtin(builtin_func builtin)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_BUILTIN;
    value->as.builtin = builtin;
    return value;
}

struct value*
value_make_lambda(struct value* params, struct value* body, struct value* env)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_LAMBDA;
    value->as.lambda.params = params;
    value->as.lambda.body = body;
    value->as.lambda.env = env;
    return value;
}

void
value_print(const struct value* value)
{
    switch (value->type) {
        case VALUE_NUMBER:
            printf("%ld", value->as.number);
            break;
        case VALUE_SYMBOL:
            printf("%s", value->as.symbol);
            break;
        case VALUE_PAIR:
            if (value->as.pair.car == NULL && value->as.pair.cdr == NULL) {
                printf("()");
                break;
            }
            printf("(");
            if (value->as.pair.car == NULL) printf("()");
            else value_print(value->as.pair.car);
            printf(" . ");
            if (value->as.pair.cdr == NULL) printf("()");
            else value_print(value->as.pair.cdr);
            printf(")");
            break;
        case VALUE_BUILTIN:
            printf("<builtin>");
            break;
        case VALUE_LAMBDA:
            printf("<lambda>");
            break;
        default:
            printf("<undefined>");
    }
}

#define cons(a,b) (value_make_pair((a), (b)))
#define car(v)    ((v)->as.pair.car)
#define cdr(v)    ((v)->as.pair.car)
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

struct value*
read(const char* str)
{
    if (*str >= '0' && *str <= '9') {
        char* end = NULL;
        long number = strtol(str, &end, 10);
        return value_make_number(number);
    }

    return value_make_pair(NULL, NULL);
}

struct value* eval(struct value* exp, struct value* env);
struct value* apply(struct value* proc, struct value* args);
struct value* evlist(struct value* exps, struct value* env);
struct value* evcond(struct value* exps, struct value* env);
struct value* lookup(struct value* exp, struct value* env);

struct value*
eval(struct value* exp, struct value* env)
{
    if (exp->type == VALUE_NUMBER) {
        return exp;
    } else if (exp->type == VALUE_SYMBOL) {
        return lookup(exp, env);
    } else if (strcmp(car(exp)->as.symbol, "quote") == 0) {
        return cadr(exp);
    } else if (strcmp(car(exp)->as.symbol, "lambda") == 0) {
        return cons(value_make_symbol("closure", 7),
                    cons(cdr(exp),
                         cons(env, value_make_pair(NULL, NULL))));
    } else if (strcmp(car(exp)->as.symbol, "cond") == 0) {
        return evcond(cdr(exp), env);
    } else {

    }

    fprintf(stderr, "unknown expression type\n");
    return NULL;
}

struct value*
apply(struct value* proc, struct value* args)
{
    if (proc->type == VALUE_BUILTIN) {

    } else if (proc->type == VALUE_LAMBDA) {

    }

    fprintf(stderr, "unknown procesdure type\n");
    return NULL;
}

struct value*
evlist(struct value* exps, struct value* env)
{
    if (car(exps) == NULL && cdr(exps) == NULL) {
        return exps;
    }

    return cons(eval(car(exps), env), evlist(cdr(exps), env));
}

struct value*
evcond(struct value* exps, struct value* env)
{
    return NULL;
}

struct value*
lookup(struct value* exp, struct value* env)
{
    return NULL;
}

int
main(int argc, char* argv[])
{
    char line[512] = { 0 };

    printf("> ");
    while (fgets(line, sizeof(line), stdin) != NULL) {
        struct value* exp = read(line);
        value_print(exp);
        printf("\n");

        printf("> ");
    }
}
