#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "linenoise.h"

static const char SQUEAKY_PROMPT[] = "squeaky> ";
static const char SQUEAKY_HISTORY_FILE[] = ".squeaky_history";

int
main(int argc, char* argv[])
{
    linenoiseHistoryLoad(SQUEAKY_HISTORY_FILE);

    puts("Welcome to Squeaky Scheme!");
    puts("Use Ctrl-c to exit.");

    char* line = NULL;
    while ((line = linenoise(SQUEAKY_PROMPT)) != NULL) {
        if (line[0] == '\0') {
            linenoiseFree(line);
            break;
        }

        linenoiseHistoryAdd(line);
        linenoiseHistorySave(SQUEAKY_HISTORY_FILE);

        puts(line);

        struct lexer l = {
            .input = line,
            .input_len = strlen(line),
        };

        int rc = LEXER_OK;
        struct token t = { 0 };
        while ((rc = lexer_lex(&l, &t)) == LEXER_OK) {
            lexer_print(&t);
        }

        if (rc == LEXER_ERROR) {
            fprintf(stderr, "!!! syntax error !!!\n");
        }

        linenoiseFree(line);
    }

    return EXIT_SUCCESS;
}
