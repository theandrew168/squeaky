#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lenv.h"
#include "linenoise.h"
#include "lval.h"
#include "mpc.h"

#define LASSERT(args, cond, err)  \
    if (!(cond)) { lval_free(args); return lval_make_error(err); }

#define LASSERTF(args, cond, fmt, ...)                           \
    if (!(cond)) {                                               \
        struct lval* err = lval_make_error(fmt, ##__VA_ARGS__);  \
        lval_free(args);                                         \
        return err;                                              \
    }

static const char SQUEAKY_PROMPT[] = "squeaky> ";
static const char SQUEAKY_HISTORY_FILE[] = ".squeaky_history";

struct lval* eval_sexpr(struct lenv* env, struct lval* val);
struct lval* eval(struct lenv* env, struct lval* val);

static struct lval*
lval_read_number(mpc_ast_t* ast)
{
    errno = 0;
    long x = strtol(ast->contents, NULL, 10);
    return errno != ERANGE ? lval_make_number(x) : lval_make_error("invalid number");
}

static struct lval*
lval_read(mpc_ast_t* ast)
{
    // if number or symbol, convert to that type
    if (strstr(ast->tag, "number")) return lval_read_number(ast);
    if (strstr(ast->tag, "symbol")) return lval_make_symbol(ast->contents);

    // if root or sexpr then create an empty list
    struct lval* list = NULL;
    if (strcmp(ast->tag, ">") == 0) list = lval_make_sexpr();
    if (strstr(ast->tag, "sexpr")) list = lval_make_sexpr();
    if (strstr(ast->tag, "qexpr")) list = lval_make_qexpr();

    // fill the list with any valid sub-expressions
    for (long i = 0; i < ast->children_num; i++) {
        if (strcmp(ast->children[i]->contents, "(") == 0) continue;
        if (strcmp(ast->children[i]->contents, ")") == 0) continue;
        if (strcmp(ast->children[i]->contents, "{") == 0) continue;
        if (strcmp(ast->children[i]->contents, "}") == 0) continue;
        if (strcmp(ast->children[i]->tag, "regex") == 0) continue;
        list = lval_list_append(list, lval_read(ast->children[i]));
    }

    return list;
}

struct lval*
builtin_op(struct lenv* env, struct lval* val, char* op)
{
    // ensure all args are numbers
    for (long i = 0; i < val->cell_count; i++) {
        LASSERTF(val, val->cell[i]->type == LVAL_TYPE_NUMBER,
            "function '%s' passed incorrect type for arg %i: "
            "want %s, got %s",
            op, i, lval_type_name(LVAL_TYPE_NUMBER), lval_type_name(val->cell[0]->type));
    }

    // pop the first element
    struct lval* x = lval_list_pop(val, 0);

    // if no args and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && val->cell_count == 0) {
        x->number = -x->number;
    }

    // mathematically condense all children into x
    while (val->cell_count > 0) {
        struct lval* y = lval_list_pop(val, 0);

        if (strcmp(op, "+") == 0) x->number += y->number;
        if (strcmp(op, "-") == 0) x->number -= y->number;
        if (strcmp(op, "*") == 0) x->number *= y->number;
        if (strcmp(op, "/") == 0) {
            if (y->number == 0) {
                lval_free(x);
                lval_free(y);
                x = lval_make_error("division by zero");
                break;
            }
            x->number /= y->number;
        }

        lval_free(y);
    }

    // free the original list and return the calculated x
    lval_free(val);
    return x;
}

struct lval*
builtin_add(struct lenv* env, struct lval* val)
{
    return builtin_op(env, val, "+");
}

struct lval*
builtin_sub(struct lenv* env, struct lval* val)
{
    return builtin_op(env, val, "-");
}

struct lval*
builtin_mul(struct lenv* env, struct lval* val)
{
    return builtin_op(env, val, "*");
}

struct lval*
builtin_div(struct lenv* env, struct lval* val)
{
    return builtin_op(env, val, "/");
}

struct lval*
builtin_list(struct lenv* env, struct lval* val)
{
    val->type = LVAL_TYPE_QEXPR;
    return val;
}

struct lval*
builtin_head(struct lenv* env, struct lval* val)
{
    LASSERTF(val, val->cell_count == 1,
        "function 'head' passed too many args: "
        "want %i, got %i",
        1, val->cell_count);
    LASSERTF(val, val->cell[0]->type == LVAL_TYPE_QEXPR,
        "function 'head' passed incorrect type for arg %i: "
        "want %s, got %s",
        0, lval_type_name(LVAL_TYPE_QEXPR), lval_type_name(val->cell[0]->type));
    LASSERT(val, val->cell[0]->cell_count != 0, "function 'head' passed empty list");

    struct lval* head = lval_list_take(val, 0);
    while (head->cell_count > 1) lval_free(lval_list_pop(head, 1));
    return head;
}

struct lval*
builtin_tail(struct lenv* env, struct lval* val)
{
    LASSERTF(val, val->cell_count == 1,
        "function 'tail' passed too many args: "
        "want %i, got %i",
        1, val->cell_count);
    LASSERTF(val, val->cell[0]->type == LVAL_TYPE_QEXPR,
        "function 'tail' passed incorrect type for arg %i: "
        "want %s, got %s",
        0, lval_type_name(LVAL_TYPE_QEXPR), lval_type_name(val->cell[0]->type));
    LASSERT(val, val->cell[0]->cell_count != 0, "function 'tail' passed empty list");

    struct lval* tail = lval_list_take(val, 0);
    lval_free(lval_list_pop(tail, 0));
    return tail;
}

struct lval*
builtin_join(struct lenv* env, struct lval* val)
{
    for (long i = 0; i < val->cell_count; i++ ) {
        LASSERTF(val, val->cell[i]->type == LVAL_TYPE_QEXPR,
            "function 'tail' passed incorrect type for arg %i: "
            "want %s, got %s",
            i, lval_type_name(LVAL_TYPE_QEXPR), lval_type_name(val->cell[i]->type));
    }

    struct lval* list = lval_list_pop(val, 0);
    while (val->cell_count) {
        list = lval_list_join(list, lval_list_pop(val, 0));
    }
    lval_free(val);

    return list;
}

struct lval*
builtin_eval(struct lenv* env, struct lval* val)
{
    LASSERTF(val, val->cell_count == 1,
        "function 'eval' passed too many args: "
        "want %i, got %i",
        1, val->cell_count);
    LASSERTF(val, val->cell[0]->type == LVAL_TYPE_QEXPR,
        "function 'eval' passed incorrect type for arg %i: "
        "want %s, got %s",
        0, lval_type_name(LVAL_TYPE_QEXPR), lval_type_name(val->cell[0]->type));

    struct lval* sexpr = lval_list_take(val, 0);
    sexpr->type = LVAL_TYPE_SEXPR;
    return eval(env, sexpr);
}

struct lval*
builtin_def(struct lenv* env, struct lval* val)
{
    LASSERTF(val, val->cell[0]->type == LVAL_TYPE_QEXPR,
        "function 'def' passed incorrect type for arg %i: "
        "want %s, got %s",
        0, lval_type_name(LVAL_TYPE_QEXPR), lval_type_name(val->cell[0]->type));

    struct lval* symbols = val->cell[0];
    for (long i = 0; i < symbols->cell_count; i++) {
        LASSERT(val, symbols->cell[i]->type == LVAL_TYPE_SYMBOL, "function 'def' cannot define non-symbol");
    }

    LASSERT(val, symbols->cell_count == val->cell_count - 1, "function 'def' passed mismatched number of symbols and valects");

    // assign copies of valects to symbols
    for (long i = 0; i < symbols->cell_count; i++) {
        lenv_put(env, symbols->cell[i], val->cell[i + 1]);
    }

    lval_free(val);
    return lval_make_sexpr();
}

void
add_builtin(struct lenv* env, const char* name, lbuiltin func)
{
    struct lval* k = lval_make_symbol(name);
    struct lval* v = lval_make_func(func);
    lenv_put(env, k, v);
    lval_free(k);
    lval_free(v);
}

void
add_builtins(struct lenv* env)
{
    add_builtin(env, "list", builtin_list);
    add_builtin(env, "head", builtin_head);
    add_builtin(env, "tail", builtin_tail);
    add_builtin(env, "join", builtin_join);
    add_builtin(env, "eval", builtin_eval);

    add_builtin(env, "+", builtin_add);
    add_builtin(env, "-", builtin_sub);
    add_builtin(env, "*", builtin_mul);
    add_builtin(env, "/", builtin_div);

    add_builtin(env, "def", builtin_def);
}

struct lval*
eval_sexpr(struct lenv* env, struct lval* val)
{
    // eval children
    for (long i = 0; i < val->cell_count; i++) {
        val->cell[i] = eval(env, val->cell[i]);
    }

    // error checking
    for (long i = 0; i < val->cell_count; i++) {
        if (val->cell[i]->type == LVAL_TYPE_ERROR) return lval_list_take(val, i);
    }

    // empty expression
    if (val->cell_count == 0) return val;

    // single expression
    if (val->cell_count == 1) return lval_list_take(val, 0);

    // ensure first element is a symbol
    struct lval* f = lval_list_pop(val, 0);
    if (f->type != LVAL_TYPE_FUNC) {
        lval_free(f);
        lval_free(val);
        return lval_make_error("s-expressions does not start with a function");
    }

    // call builtin with operator
    struct lval* result = f->func(env, val);
    lval_free(f);
    return result;
}

struct lval*
eval(struct lenv* env, struct lval* val)
{
    if (val->type == LVAL_TYPE_SYMBOL) {
        struct lval* v = lenv_get(env, val);
        lval_free(val);
        return v;
    }

    if (val->type == LVAL_TYPE_SEXPR) {
        return eval_sexpr(env, val);
    }

    return val;
}

int
main(int argc, char* argv[])
{
    mpc_parser_t* Number  = mpc_new("number");
    mpc_parser_t* Symbol  = mpc_new("symbol");
    mpc_parser_t* Sexpr   = mpc_new("sexpr");
    mpc_parser_t* Qexpr   = mpc_new("qexpr");
    mpc_parser_t* Expr    = mpc_new("expr");
    mpc_parser_t* Squeaky = mpc_new("squeaky");

    mpca_lang(MPCA_LANG_DEFAULT,
        "number  : /-?[0-9]+/ ;"
        "symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;"
        "sexpr   : '(' <expr>* ')' ;"
        "qexpr   : '{' <expr>* '}' ;"
        "expr    : <number> | <symbol> | <sexpr> | <qexpr> ;"
        "squeaky : /^/ <expr>* /$/ ;",
        Number, Symbol, Sexpr, Qexpr, Expr, Squeaky, NULL);

    puts("Welcome to Squeaky Scheme!");
    puts("Use Ctrl-c to exit.");

    linenoiseHistoryLoad(SQUEAKY_HISTORY_FILE);

    struct lenv* env = lenv_make();
    add_builtins(env);

    char* line = NULL;
    while ((line = linenoise(SQUEAKY_PROMPT)) != NULL) {
        if (line[0] != '\0') {
            linenoiseHistoryAdd(line);
            linenoiseHistorySave(SQUEAKY_HISTORY_FILE);
        }

        mpc_result_t r = { 0 };
        if (mpc_parse("<stdin>", line, Squeaky, &r) != 0) {
            struct lval* res = eval(env, lval_read(r.output));
            lval_println(res);
            lval_free(res);

            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        linenoiseFree(line);
    }

    lenv_free(env);
    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Squeaky);
    return EXIT_SUCCESS;
}
