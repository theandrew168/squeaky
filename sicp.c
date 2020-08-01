#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. Define core data structure (linkable tagged union)
// 2. Convert text to this data structure (read)
// 3. Evaluate the data structure (eval/apply)

// BUG: Calling nested lambda reads invalid memory
//  (define square (lambda (x) (* x x)))
//  (define quad (lambda (x) (* square(x) square(x))))
//  (quad 5)  ; gives unpredictable results

// TODO: Add env creation helper (list of [sym, func] structs?)
// TODO: Add read_list helper to read func
// TODO: Add special form "if"
// TODO: Add assert helpers for builtins (arity and types)
// TODO: Add a simple ref counted GC / memory management

// value.c/.h
// ----------
// value_type enum
// builtin_func typedef
// value tagged union
// value_is_foo macros
// value_make_foo constructors
// value_free destructor
// value_read / value_write
//
// list.c/.h
// ---------
// cons / car / cdr macros
// list creation helper
// list length helper
//
// env.c/.h
// --------
// env creation helper
// env print helper
// env_bind
// env_lookup
// env_define
// env_update
//
// builtin.c/.h
// ------------
// builtin helper assertions (arity and type)
// builtin functions (plus, multiply, etc)
//
// main.c (for now)
// ------
// read / write / eval / apply

enum value_type {
    VALUE_UNDEFINED = 0,
    VALUE_BOOLEAN,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_SYMBOL,
    VALUE_PAIR,
    VALUE_BUILTIN,
    VALUE_LAMBDA,
    VALUE_ERROR,
};

struct value;
typedef struct value* (*builtin_func)(struct value* args);

struct value {
    int type;
    union {
        bool boolean;
        long number;
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
            struct value* env;
        } lambda;
        char* error;
    } as;
};

struct value* value_make_boolean(bool boolean);
struct value* value_make_number(long number);
struct value* value_make_string(const char* string, long length);
struct value* value_make_symbol(const char* symbol, long length);
struct value* value_make_pair(struct value* car, struct value* cdr);
struct value* value_make_builtin(builtin_func builtin);
struct value* value_make_lambda(struct value* params, struct value* body, struct value* env);
struct value* value_make_error(const char* error);
void value_free(struct value* value);
struct value* value_read(const char* str, long* consumed);
void value_write(const struct value* value);

struct value*
value_make_boolean(bool boolean)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_BOOLEAN;
    value->as.boolean = boolean;
    return value;
}

struct value*
value_make_number(long number)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_NUMBER;
    value->as.number = number;
    return value;
}

struct value*
value_make_string(const char* string, long length)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_STRING;
    value->as.string = malloc(length - 2 + 1); // leave out the quotes
    strncpy(value->as.string, string + 1, length - 2);
    value->as.string[length - 2] = '\0';
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

struct value*
value_make_error(const char* error)
{
    struct value* value = malloc(sizeof(struct value));
    value->type = VALUE_ERROR;
    value->as.error = malloc(strlen(error) + 1);
    strcpy(value->as.error, error);
    return value;
}

void
value_free(struct value* value)
{
    if (value == NULL) return;

    switch (value->type) {
        case VALUE_BOOLEAN:
            break;
        case VALUE_NUMBER:
            break;
        case VALUE_STRING:
            free(value->as.string);
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
        case VALUE_ERROR:
            free(value->as.error);
            break;
    }

    free(value);
}

#define value_is_boolean(v) ((v)->type == VALUE_BOOLEAN)
#define value_is_number(v)  ((v)->type == VALUE_NUMBER)
#define value_is_string(v)  ((v)->type == VALUE_STRING)
#define value_is_symbol(v)  ((v)->type == VALUE_SYMBOL)
#define value_is_pair(v)    ((v)->type == VALUE_PAIR)
#define value_is_builtin(v) ((v)->type == VALUE_BUILTIN)
#define value_is_lambda(v)  ((v)->type == VALUE_LAMBDA)
#define value_is_error(v)   ((v)->type == VALUE_ERROR)

bool
value_is_true(struct value* exp)
{
    return value_is_boolean(exp) &&
           exp->as.boolean == true;
}

bool
value_is_false(struct value* exp)
{
    return value_is_boolean(exp) &&
           exp->as.boolean == false;
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

struct value* list_make(struct value* value, ...);
long list_length(const struct value* list);

struct value*
list_make(struct value* value, ...)
{
    va_list args;
    va_start(args, value);

    struct value* head = cons(value, NULL);
    struct value* tail = head;
    for (;;) {
        struct value* v = va_arg(args, struct value*);
        if (v == NULL) break;

        cdr(tail) = cons(v, NULL);
        tail = cdr(tail);
    }

    va_end(args);
    return head;
}

long
list_length(const struct value* list)
{
    long count = 0;

    const struct value* iter = list;
    while (iter != NULL) {
        count++;
        iter = cdr(iter);
    }

    return count;
}

struct value* env_bind(struct value* vars, struct value* vals, struct value* env);
struct value* env_lookup(struct value* sym, struct value* env);
struct value* env_update(struct value* sym, struct value* value, struct value* env);
struct value* env_define(struct value* sym, struct value* value, struct value* env);
void env_print(const struct value* env);

static struct value*
pair_up(struct value* vars, struct value* vals)
{
    if (vars == NULL && vals == NULL) return NULL;
    if (vars == NULL) {
        fprintf(stderr, "too many arguments given to lambda... crash!\n");
        return value_make_error("too many arguments given to lambda");
    }
    if (vals == NULL) {
        fprintf(stderr, "too few arguments given to lambda... crash!\n");
        return value_make_error("too few arguments given to lambda");
    }

    return cons(cons(car(vars), car(vals)),
                pair_up(cdr(vars), cdr(vals)));
}

// create a new env with a leading frame that binds vars to vals
struct value*
env_bind(struct value* vars, struct value* vals, struct value* env)
{
    return cons(pair_up(vars, vals), env);
}

// search a frame for a given symbol, return (sym, value) pair if found
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

// recur search all frames
struct value*
env_lookup(struct value* sym, struct value* env)
{
    if (env == NULL) {
        return value_make_error("unbound variable");
    }

    struct value* vcell = assq(sym, car(env));
    if (vcell != NULL) {
        return cdr(vcell);
    } else {
        return env_lookup(sym, cdr(env));
    }
}

// recur serach and update, error if not found
struct value*
env_update(struct value* sym, struct value* value, struct value* env)
{
    if (env == NULL) {
        return value_make_error("unbound variable");
    }

    struct value* vcell = assq(sym, car(env));
    if (vcell != NULL) {
        cdr(vcell) = value;
        return value_make_pair(NULL, NULL);
    } else {
        return env_lookup(sym, cdr(env));
    }
}

// search first frame only, update if found, add if not
struct value*
env_define(struct value* sym, struct value* value, struct value* env)
{
    struct value* vcell = assq(sym, car(env));
    if (vcell != NULL) {
        // update exiting vcell
        cdr(vcell) = value;
    } else {
        // prepend a new vcell to this frame
        car(env) = cons(cons(sym, value), car(env));
    }

    return value_make_pair(NULL, NULL);
}

void
env_print(const struct value* env)
{
    if (env == NULL) return;
    env_print(cdr(env));

    struct value* frame = car(env);
    while (frame != NULL) {
        struct value* vcell = car(frame);
        printf("env| %s: ", car(vcell)->as.symbol);
        value_write(cdr(vcell));
        printf("\n");

        frame = cdr(frame);
    }

    printf("env| == frame == \n");
}

struct value* builtin_plus(struct value* args);
struct value* builtin_multiply(struct value* args);

struct value*
builtin_plus(struct value* args)
{
    long sum = 0;
    for (; args != NULL; args = cdr(args)) {
        sum += car(args)->as.number;
    }

    return value_make_number(sum);
}

struct value*
builtin_multiply(struct value* args)
{
    long product = 1;
    for (; args != NULL; args = cdr(args)) {
        product *= car(args)->as.number;
    }

    return value_make_number(product);
}

struct value* eval(struct value* exp, struct value* env);
struct value* apply(struct value* proc, struct value* args);

struct value*
value_read(const char* str, long* consumed)
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
        *consumed = start - str;
        return value_make_error("unexpected EOF");
    }

    // boolean
    // TODO: character "#\"
    // TODO: vector    "#("
    if (*start == '#') {
        const char* iter = start;
        iter++;
        if (strchr("tf", *iter)) {
            bool boolean = *iter == 't';
            iter++;
            *consumed = iter - str;
            return value_make_boolean(boolean);
        }

        return value_make_error("invalid expression");
    }

    // number
    if (strchr(digit, *start)) {
        char* iter = NULL;
        long number = strtol(start, &iter, 10);
        *consumed = iter - str;
        return value_make_number(number);
    }

    // string
    if (*start == '"') {
        const char* iter = start;
        iter++;  // consume open quote

        while (*iter != '"' && *iter != '\0') iter++;
        if (*iter == '\0') return value_make_error("unterminated string");

        iter++;  // consume close quote
        *consumed = iter - str;
        return value_make_string(start, iter - start);
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
        const char* iter = start;
        iter++;  // skip open paren

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
            struct value* cell = cons(value_read(iter, consumed), NULL);

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

    return value_make_error("invalid expression");
}

// but pair would need to be simplified once all
//   the cars / cdrs are settled
void
value_write(const struct value* value)
{
    assert(value != NULL);

    switch (value->type) {
        case VALUE_BOOLEAN:
            printf("%s", value->as.boolean ? "#t" : "#f");
            break;
        case VALUE_NUMBER:
            printf("%ld", value->as.number);
            break;
        case VALUE_STRING:
            printf("\"%s\"", value->as.string);
            break;
        case VALUE_SYMBOL:
            printf("%s", value->as.symbol);
            break;
        case VALUE_PAIR:
            if (value->as.pair.car == NULL && value->as.pair.cdr == NULL) {
                printf("ok");
                break;
            }
            printf("(");
            if (value->as.pair.car == NULL) printf("'()");
            else value_write(value->as.pair.car);
            printf(" . ");
            if (value->as.pair.cdr == NULL) printf("'()");
            else value_write(value->as.pair.cdr);
            printf(")");
            break;
        case VALUE_BUILTIN:
            printf("<builtin>");
            break;
        case VALUE_LAMBDA:
            printf("<lambda>");
            break;
        case VALUE_ERROR:
            printf("error: %s", value->as.error);
            break;
        default:
            printf("<undefined>");
    }
}

struct value* eval(struct value* exp, struct value* env);
struct value* apply(struct value* proc, struct value* args);
struct value* evlist(struct value* exps, struct value* env);
struct value* evcond(struct value* exps, struct value* env);

struct value*
eval(struct value* exp, struct value* env)
{
    if (exp->type == VALUE_BOOLEAN) {
        return exp;
    } else if (exp->type == VALUE_NUMBER) {
        return exp;
    } else if (exp->type == VALUE_STRING) {
        return exp;
    } else if (exp->type == VALUE_ERROR) {
        return exp;
    } else if (exp->type == VALUE_SYMBOL) {
        return env_lookup(exp, env);
    } else if (strcmp(car(exp)->as.symbol, "quote") == 0) {
        return cadr(exp);
    } else if (strcmp(car(exp)->as.symbol, "lambda") == 0) {
        // this differs from SICP:
        // the lecture returns: ('closure (params body) env)
        // (lambda (x) (* x x))
//        struct value* lamb = value_make_lambda(cadr(exp), caddr(exp), env);
//        value_write(lamb->as.lambda.params);
//        printf("\n");
//        value_write(lamb->as.lambda.body);
//        printf("\n");
//        env_print(lamb->as.lambda.env);
//        return lamb;
        return value_make_lambda(cadr(exp), caddr(exp), env);
    } else if (strcmp(car(exp)->as.symbol, "cond") == 0) {
        return evcond(cdr(exp), env);
    } else if (strcmp(car(exp)->as.symbol, "define") == 0) {
        return env_define(cadr(exp), eval(caddr(exp), env), env);
    } else if (strcmp(car(exp)->as.symbol, "set!") == 0) {
        return env_update(cadr(exp), eval(caddr(exp), env), env);
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
//        struct value* bound_env = env_bind(proc->as.lambda.params, args, proc->as.lambda.env);
//        env_print(bound_env);
//        return eval(proc->as.lambda.body, bound_env);
        return eval(proc->as.lambda.body, env_bind(proc->as.lambda.params, args, proc->as.lambda.env));
    } else {
        return value_make_error("unknown procedure type");
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
    } else if (value_is_false(eval(caar(exps), env))) {
        return evcond(cdr(exps), env);
    } else {
        return eval(cadar(exps), env);
    }
}

int
//main(int argc, char* argv[])
main(void)
{
    struct value* vars = list_make(
        value_make_symbol("+", 1),
        value_make_symbol("*", 1),
        NULL);
    struct value* vals = list_make(
        value_make_builtin(builtin_plus),
        value_make_builtin(builtin_multiply),
        NULL);
    struct value* env = env_bind(vars, vals, NULL);

//    env_print(env);

    printf("> ");
    char line[512] = { 0 };
    while (fgets(line, sizeof(line), stdin) != NULL) {
        long consumed = 0;
        struct value* exp = value_read(line, &consumed);
//        write(exp);
//        printf("\n");

        struct value* res = eval(exp, env);
        value_write(res);
        printf("\n");

        printf("> ");
    }
}
