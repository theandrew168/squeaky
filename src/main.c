#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"
#include "lval.h"
#include "mpc.h"

static const char LISPY_PROMPT[] = "lispy> ";
static const char LISPY_HISTORY_FILE[] = ".lispy_history";

static lval
eval_op(lval x, char* op, lval y)
{
    // if either value is an error, return it
    if (x.type == LVAL_ERR) return x;
    if (y.type == LVAL_ERR) return x;

    // otherwise to the math
    if (strcmp(op, "+") == 0) return lval_num(x.num + y.num);
    if (strcmp(op, "-") == 0) return lval_num(x.num - y.num);
    if (strcmp(op, "*") == 0) return lval_num(x.num * y.num);
    if (strcmp(op, "/") == 0) {
        if (y.num == 0) return lval_err(LERR_DIV_ZERO);
        return lval_num(x.num / y.num);
    }

    return lval_err(LERR_BAD_OP);
}

static lval
eval(mpc_ast_t* t)
{
    // if node is a number, return the number
    if (strstr(t->tag, "number")) {
        // check for errors in the string -> number conversion
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    // operator is always the second child
    char* op = t->children[1]->contents;

    // store the third child in 'x'
    lval x = eval(t->children[2]);

    // iterate over and combine the rest of the children
    long i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }

    return x;
}

int
main(int argc, char* argv[])
{
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Lispy    = mpc_new("lispy");
    
    mpca_lang(MPCA_LANG_DEFAULT,
        "number   : /-?[0-9]+/ ;"
        "operator : '+' | '-' | '*' | '/' ;"
        "expr     : <number> | '(' <operator> <expr>+ ')' ;"
        "lispy    : /^/ <operator> <expr>+ /$/ ;",
        Number, Operator, Expr, Lispy);

    puts("Lispy Version 0.0.1");
    puts("Press Ctrl+c to exit\n");

    linenoiseHistoryLoad(LISPY_HISTORY_FILE);

    char* line = NULL;
    while ((line = linenoise(LISPY_PROMPT)) != NULL) {
        if (line[0] == '\0') {
            linenoiseFree(line);
            break;
        }

        linenoiseHistoryAdd(line);
        linenoiseHistorySave(LISPY_HISTORY_FILE);

        mpc_result_t r = { 0 };
        if (mpc_parse("<stdin>", line, Lispy, &r) != 0) {
            lval result = eval(r.output);
            lval_println(result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        linenoiseFree(line);
    }

    mpc_cleanup(4, Number, Operator, Expr, Lispy);

    return EXIT_SUCCESS;
}
