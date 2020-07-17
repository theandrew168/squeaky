#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"
#include "lobj.h"
#include "mpc.h"

#define LASSERT(args, cond, err)  \
    if (!(cond)) { lobj_free(args); return lobj_make_error(err); }

static const char SQUEAKY_PROMPT[] = "squeaky> ";
static const char SQUEAKY_HISTORY_FILE[] = ".squeaky_history";

struct lobj* eval_sexpr(struct lobj* obj);
struct lobj* eval(struct lobj* obj);

static struct lobj*
lobj_read_number(mpc_ast_t* ast)
{
    errno = 0;
    long x = strtol(ast->contents, NULL, 10);
    return errno != ERANGE ? lobj_make_number(x) : lobj_make_error("invalid number");
}

static struct lobj*
lobj_read(mpc_ast_t* ast)
{
    // if number or symbol, convert to that type
    if (strstr(ast->tag, "number")) return lobj_read_number(ast);
    if (strstr(ast->tag, "symbol")) return lobj_make_symbol(ast->contents);

    // if root or sexpr then create an empty list
    struct lobj* list = NULL;
    if (strcmp(ast->tag, ">") == 0) list = lobj_make_sexpr();
    if (strstr(ast->tag, "sexpr")) list = lobj_make_sexpr();
    if (strstr(ast->tag, "qexpr")) list = lobj_make_qexpr();

    // fill the list with any valid sub-expressions
    for (long i = 0; i < ast->children_num; i++) {
        if (strcmp(ast->children[i]->contents, "(") == 0) continue;
        if (strcmp(ast->children[i]->contents, ")") == 0) continue;
        if (strcmp(ast->children[i]->contents, "{") == 0) continue;
        if (strcmp(ast->children[i]->contents, "}") == 0) continue;
        if (strcmp(ast->children[i]->tag, "regex") == 0) continue;
        list = lobj_list_append(list, lobj_read(ast->children[i]));
    }

    return list;
}

struct lobj*
builtin_op(struct lobj* obj, char* op)
{
    // ensure all args are numbers
    for (long i = 0; i < obj->cell_count; i++) {
        if (obj->cell[i]->type != LOBJ_TYPE_NUMBER) {
            lobj_free(obj);
            return lobj_make_error("cannot operate on non-number");
        }
    }

    // pop the first element
    struct lobj* x = lobj_list_pop(obj, 0);

    // if no args and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && obj->cell_count == 0) {
        x->number = -x->number;
    }

    // mathematically condense all children into x
    while (obj->cell_count > 0) {
        struct lobj* y = lobj_list_pop(obj, 0);

        if (strcmp(op, "+") == 0) x->number += y->number;
        if (strcmp(op, "-") == 0) x->number -= y->number;
        if (strcmp(op, "*") == 0) x->number *= y->number;
        if (strcmp(op, "/") == 0) {
            if (y->number == 0) {
                lobj_free(x);
                lobj_free(y);
                x = lobj_make_error("division by zero");
                break;
            }
            x->number /= y->number;
        }

        lobj_free(y);
    }

    // free the original list and return the calculated x
    lobj_free(obj);
    return x;
}

struct lobj*
builtin_list(struct lobj* obj)
{
    obj->type = LOBJ_TYPE_QEXPR;
    return obj;
}

struct lobj*
builtin_head(struct lobj* obj)
{
    LASSERT(obj, obj->cell_count == 1, "function 'head' passed too many args");
    LASSERT(obj, obj->cell[0]->type == LOBJ_TYPE_QEXPR, "function 'head' passed incorrect type");
    LASSERT(obj, obj->cell[0]->cell_count != 0, "function 'head' passed empty list");

    struct lobj* head = lobj_list_take(obj, 0);
    while (head->cell_count > 1) lobj_free(lobj_list_pop(head, 1));
    return head;
}

struct lobj*
builtin_tail(struct lobj* obj)
{
    LASSERT(obj, obj->cell_count == 1, "function 'tail' passed too many args");
    LASSERT(obj, obj->cell[0]->type == LOBJ_TYPE_QEXPR, "function 'tail' passed incorrect type");
    LASSERT(obj, obj->cell[0]->cell_count != 0, "function 'tail' passed empty list");

    struct lobj* tail = lobj_list_take(obj, 0);
    lobj_free(lobj_list_pop(tail, 0));
    return tail;
}

struct lobj*
builtin_join(struct lobj* obj)
{
    for (long i = 0; i < obj->cell_count; i++ ) {
        LASSERT(obj, obj->cell[i]->type == LOBJ_TYPE_QEXPR, "function 'join' passed incorrect type");
    }

    struct lobj* list = lobj_list_pop(obj, 0);
    while (obj->cell_count) {
        list = lobj_list_join(list, lobj_list_pop(obj, 0));
    }
    lobj_free(obj);

    return list;
}

struct lobj*
builtin_eval(struct lobj* obj)
{
    LASSERT(obj, obj->cell_count == 1, "function 'eval' passed too many args");
    LASSERT(obj, obj->cell[0]->type == LOBJ_TYPE_QEXPR, "function 'eval' passed incorrect type");

    struct lobj* sexpr = lobj_list_take(obj, 0);
    sexpr->type = LOBJ_TYPE_SEXPR;
    return eval(sexpr);
}

struct lobj*
builtin(struct lobj* obj, char* func)
{
    if (strcmp("list", func) == 0) return builtin_list(obj);
    if (strcmp("head", func) == 0) return builtin_head(obj);
    if (strcmp("tail", func) == 0) return builtin_tail(obj);
    if (strcmp("join", func) == 0) return builtin_join(obj);
    if (strcmp("eval", func) == 0) return builtin_eval(obj);
    if (strstr("+-/*", func)) return builtin_op(obj, func);
    lobj_free(obj);
    return lobj_make_error("unknown function");
}

struct lobj*
eval_sexpr(struct lobj* obj)
{
    // eval children
    for (long i = 0; i < obj->cell_count; i++) {
        obj->cell[i] = eval(obj->cell[i]);
    }

    // error checking
    for (long i = 0; i < obj->cell_count; i++) {
        if (obj->cell[i]->type == LOBJ_TYPE_ERROR) return lobj_list_take(obj, i);
    }

    // empty expression
    if (obj->cell_count == 0) return obj;

    // single expression
    if (obj->cell_count == 1) return lobj_list_take(obj, 0);

    // ensure first element is a symbol
    struct lobj* f = lobj_list_pop(obj, 0);
    if (f->type != LOBJ_TYPE_SYMBOL) {
        lobj_free(f);
        lobj_free(obj);
        return lobj_make_error("s-expressions does not start with symbol");
    }

    // call builtin with operator
    struct lobj* result = builtin(obj, f->symbol);
    lobj_free(f);
    return result;
}

struct lobj*
eval(struct lobj* obj)
{
    if (obj->type == LOBJ_TYPE_SEXPR) return eval_sexpr(obj);
    return obj;
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
        "symbol  : \"list\" | \"head\" | \"tail\" "
        "        | \"join\" | \"eval\" "
        "        | '+' | '-' | '*' | '/' ;"
        "sexpr   : '(' <expr>* ')' ;"
        "qexpr   : '{' <expr>* '}' ;"
        "expr    : <number> | <symbol> | <sexpr> | <qexpr> ;"
        "squeaky : /^/ <expr>* /$/ ;",
        Number, Symbol, Sexpr, Qexpr, Expr, Squeaky, NULL);

    puts("Welcome to Squeaky Scheme!");
    puts("Use Ctrl-c to exit.");

    linenoiseHistoryLoad(SQUEAKY_HISTORY_FILE);

    char* line = NULL;
    while ((line = linenoise(SQUEAKY_PROMPT)) != NULL) {
        if (line[0] != '\0') {
            linenoiseHistoryAdd(line);
            linenoiseHistorySave(SQUEAKY_HISTORY_FILE);
        }

        mpc_result_t r = { 0 };
        if (mpc_parse("<stdin>", line, Squeaky, &r) != 0) {
            struct lobj* res = eval(lobj_read(r.output));
            lobj_println(res);
            lobj_free(res);

            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        linenoiseFree(line);
    }

    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Squeaky);
    return EXIT_SUCCESS;
}
