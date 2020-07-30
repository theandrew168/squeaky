#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. Define core data structure (linkable tagged union)
// 2. Convert text to this data structure (read)
// 3. Evaluate the data structure (eval/apply)

enum value_type {
    VALUE_UNDEFINED = 0,
    VALUE_NUMBER,
    VALUE_SYMBOL,
    VALUE_PAIR,
    VALUE_BUILTIN,
    VALUE_LAMBDA,
};

struct value;
typedef struct value* (*builtin_func)(struct value* args);

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
value_free(struct value* value)
{
    if (value == NULL) return;

    switch (value->type) {
        case VALUE_NUMBER:
            break;
        case VALUE_SYMBOL:
            free(value->as.symbol);
            break;
        case VALUE_PAIR:
            value_free(value->as.pair.car);
            value_free(value->as.pair.cdr);
            break;
        case VALUE_BUILTIN:
            break;
        case VALUE_LAMBDA:
            value_free(value->as.lambda.params);
            value_free(value->as.lambda.body);
            value_free(value->as.lambda.env);
            break;
    }

    free(value);
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
                printf("EMPTY");
                break;
            }
            printf("(");
            if (value->as.pair.car == NULL) printf("'()");
            else value_print(value->as.pair.car);
            printf(" . ");
            if (value->as.pair.cdr == NULL) printf("'()");
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
#define cdr(v)    ((v)->as.pair.cdr)
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

struct value* env_get(struct value* k, struct value* env);
void env_set(struct value* k, struct value* v, struct value* env);
void env_def(struct value* k, struct value* v, struct value* env);

struct value*
read(const char* str, long* consumed)
{
    const char space[] = " \f\n\r\t\v;";
    const char digit[] = "0123456789";
    const char alpha[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "+-!$%&*/:<=>?^_~";

    // needed to capture how many chars this value consumes
    const char* start = str;

    // whitespace / comments
    while (strchr(space, *start) && *start != '\0') {
        if (*start == ';') {
            while (*start != '\n' && *start != '\0') start++;
        }
        start++;
    }

    // EOF
    if (*start == '\0') {
        assert(0 && "unexpected EOF");
    }

    // number
    if (strchr(digit, *start)) {
        char* iter = NULL;
        long number = strtol(start, &iter, 10);
        *consumed = iter - str;
        return value_make_number(number);
    }

    // symbol
    if (strchr(alpha, *start)) {
        const char* iter = start;
        while (strchr(alpha, *iter)) iter++;
        *consumed = iter - str;
        return value_make_symbol(start, iter - start);
    }

    // pair / list / s-expression
    // TODO: this case is ugly, need to read hella book
    if (*start == '(') {
        const char* iter = start + 1;

        // whitespace / comments (again)
        while (strchr(space, *iter) && *iter != '\0') {
            if (*iter == ';') {
                while (*iter != '\n' && *iter != '\0') iter++;
            }
            iter++;
        }

        struct value* list = NULL;
        while (*iter != ')') {

            // read the next value
            struct value* cell = cons(read(iter, consumed), NULL);

            // if first child, replace the list ptr
            if (list == NULL) {
                list = cell;
            } else {  // else cons like normal
                struct value* last = list;
                while (cdr(last) != NULL) last = cdr(last);
                cdr(last) = cell;
            }

            // advance to end of read value
            iter += *consumed;

            // whitespace / comments (AGAIN)
            while (strchr(space, *iter) && *iter != '\0') {
                if (*iter == ';') {
                    while (*iter != '\n' && *iter != '\0') iter++;
                }
                iter++;
            }
        }

        iter++;  // move past the closing paren
        *consumed = iter - str;

        // special case if list never got built
        if (list == NULL) return cons(NULL, NULL);

        return list;
    }

    assert(0 && "invalid expression");
}

struct value* eval(struct value* exp, struct value* env);
struct value* apply(struct value* proc, struct value* args);
struct value* evlist(struct value* exps, struct value* env);
struct value* evcond(struct value* exps, struct value* env);
struct value* lookup(struct value* sym, struct value* env);
struct value* bind(struct value* vars, struct value* vals, struct value* env);

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
        // this differs from SICP:
        // the lecture returns: ('closure (params body) env)
        // (lambda (x) (* x x))
        return value_make_lambda(cadr(exp), caddr(exp), env);
    } else if (strcmp(car(exp)->as.symbol, "cond") == 0) {
        return evcond(cdr(exp), env);
    } else {
        return apply(eval(car(exp), env), evlist(cdr(exp), env));
    }
}

struct value*
apply(struct value* proc, struct value* args)
{
    if (proc->type == VALUE_BUILTIN) {
        // simply call the builtin
        return proc->as.builtin(args);
    } else if (proc->type == VALUE_LAMBDA) {
        // eval the lambda body in a new env that
        // binds the params to these args in a new frame
        // on top of the lambda's initial env
        return eval(proc->as.lambda.body, bind(proc->as.lambda.params, args, proc->as.lambda.env));
    } else {
        assert(0 && "unknown procedure type");
    }
}

struct value*
evlist(struct value* exps, struct value* env)
{
    if (exps == NULL) return NULL;

    return cons(eval(car(exps), env), evlist(cdr(exps), env));
}

struct value*
evcond(struct value* exps, struct value* env)
{
    if (exps == NULL) return NULL;

    if (caar(exps)->type == VALUE_SYMBOL &&
        strcmp(caar(exps)->as.symbol, "else") == 0) {
        return eval(cadar(exps), env);
    } else if (eval(caar(exps), env) == NULL) {
        return evcond(cdr(exps), env);
    } else {
        return eval(cadar(exps), env);
    }
}

static struct value*
assq(struct value* sym, struct value* frame)
{
    if (frame == NULL) return NULL;

    // TODO: type assertion for the symbols?

    if (strcmp(sym->as.symbol, caar(frame)->as.symbol) == 0) {
        return car(frame);
    } else {
        return assq(sym, cdr(frame));
    }
}

struct value*
lookup(struct value* sym, struct value* env)
{
    if (env == NULL) {
        assert(0 && "unbound variable");
    }

    struct value* vcell = assq(sym, car(env));
    if (vcell != NULL) {
        return cdr(vcell);
    } else {
        return lookup(sym, cdr(env));
    }
}

static struct value*
pair_up(struct value* vars, struct value* vals)
{
    if (vars == NULL && vals == NULL) return NULL;
    if (vars == NULL) {
        assert(0 && "too many arguments given to lambda");
    }
    if (vals == NULL) {
        assert(0 && "too few arguments given to lambda");
    }

    return cons(cons(car(vars), car(vals)),
                pair_up(cdr(vars), cdr(vals)));
}

struct value*
bind(struct value* vars, struct value* vals, struct value* env)
{
    return cons(pair_up(vars, vals), env);
}

struct value*
b_plus(struct value* args)
{
    long sum = 0;
    for (; args != NULL; args = cdr(args)) {
        sum += car(args)->as.number;
    }

    return value_make_number(sum);
}

struct value*
b_multiply(struct value* args)
{
    long product = 1;
    for (; args != NULL; args = cdr(args)) {
        product *= car(args)->as.number;
    }

    return value_make_number(product);
}

// TODO: list creation helper
// TODO: list len helper
// TODO: env helper: get - recur search all frames
// TODO: env helper: set - recur serach and update, error if not found
// TODO: env helper: def - search first frame only, update if found, add if not

int
//main(int argc, char* argv[])
main(void)
{
    struct value* env = bind(
        cons(value_make_symbol("+", 1), 
             cons(value_make_symbol("*", 1), NULL)),
        cons(value_make_builtin(b_plus),
             cons(value_make_builtin(b_multiply), NULL)),
        NULL);

    printf("> ");
    char line[512] = { 0 };
    while (fgets(line, sizeof(line), stdin) != NULL) {
        long consumed = 0;
        struct value* exp = read(line, &consumed);
        value_print(exp);
        printf("\n");

        struct value* res = eval(exp, env);
        value_print(res);
        printf("\n");

        printf("> ");
    }

    value_free(env);
}
