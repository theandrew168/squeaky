#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"
#include "lobj.h"
#include "mpc.h"

static const char SQUEAKY_PROMPT[] = "squeaky> ";
static const char SQUEAKY_HISTORY_FILE[] = ".squeaky_history";

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
    struct lobj* sexpr = NULL;
    if (strcmp(ast->tag, ">") == 0) sexpr = lobj_make_sexpr();
    if (strstr(ast->tag, "sexpr")) sexpr = lobj_make_sexpr();

    // fill the list with any valid sub-expressions
    for (long i = 0; i < ast->children_num; i++) {
        if (strcmp(ast->children[i]->contents, "(") == 0) continue;
        if (strcmp(ast->children[i]->contents, ")") == 0) continue;
        if (strcmp(ast->children[i]->tag, "regex") == 0) continue;
        lobj_sexpr_append(sexpr, lobj_read(ast->children[i]));
    }

    return sexpr;
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
    struct lobj* x = lobj_sexpr_pop(obj, 0);

    // if no args and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && obj->cell_count == 0) {
        x->number = -x->number;
    }

    // mathematically condense all children into x
    while (obj->cell_count > 0) {
        struct lobj* y = lobj_sexpr_pop(obj, 0);

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

struct lobj* eval_sexpr(struct lobj* obj);
struct lobj* eval(struct lobj* obj);

struct lobj*
eval_sexpr(struct lobj* obj)
{
    // eval children
    for (long i = 0; i < obj->cell_count; i++) {
        obj->cell[i] = eval(obj->cell[i]);
    }

    // error checking
    for (long i = 0; i < obj->cell_count; i++) {
        if (obj->cell[i]->type == LOBJ_TYPE_ERROR) return lobj_sexpr_take(obj, i);
    }

    // empty expression
    if (obj->cell_count == 0) return obj;

    // single expression
    if (obj->cell_count == 1) return lobj_sexpr_take(obj, 0);

    // ensure first element is a symbol
    struct lobj* f = lobj_sexpr_pop(obj, 0);
    if (f->type != LOBJ_TYPE_SYMBOL) {
        lobj_free(f);
        lobj_free(obj);
        return lobj_make_error("s-expressions does not start with symbol");
    }

    // call builtin with operator
    struct lobj* result = builtin_op(obj, f->symbol);
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
    mpc_parser_t* Expr    = mpc_new("expr");
    mpc_parser_t* Squeaky = mpc_new("squeaky");

    mpca_lang(MPCA_LANG_DEFAULT,
        "number  : /-?[0-9]+/ ;"
        "symbol  : '+' | '-' | '*' | '/' ;"
        "sexpr   : '(' <expr>* ')' ;"
        "expr    : <number> | <symbol> | <sexpr> ;"
        "squeaky : /^/ <expr>* /$/ ;",
        Number, Symbol, Sexpr, Expr, Squeaky, NULL);

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
            struct lobj* x = eval(lobj_read(r.output));
            lobj_println(x);
            lobj_free(x);

            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        linenoiseFree(line);
    }

    mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Squeaky);
    return EXIT_SUCCESS;
}
