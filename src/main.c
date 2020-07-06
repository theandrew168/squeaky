#include <stdio.h>
#include <stdlib.h>

#include "linenoise.h"
#include "mpc.h"

static const char LISPY_PROMPT[] = "lispy> ";
static const char LISPY_HISTORY_FILE[] = ".lispy_history";

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
            mpc_ast_print(r.output);
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
