#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"
#include "mpc.h"

static const char SQUEAKY_PROMPT[] = "squeaky> ";
static const char SQUEAKY_HISTORY_FILE[] = ".squeaky_history";

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
        Number, Symbol, Sexpr, Expr, Squeaky);

    puts("Welcome to Squeaky Scheme!");
    puts("Use Ctrl-c to exit.");

    linenoiseHistoryLoad(SQUEAKY_HISTORY_FILE);

    char* line = NULL;
    while ((line = linenoise(SQUEAKY_PROMPT)) != NULL) {
        if (line[0] == '\0') {
            linenoiseFree(line);
            break;
        }

        linenoiseHistoryAdd(line);
        linenoiseHistorySave(SQUEAKY_HISTORY_FILE);

        mpc_result_t r = { 0 };
        if (mpc_parse("<stdin>", line, Squeaky, &r) != 0) {
            mpc_ast_print(r.output);
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
