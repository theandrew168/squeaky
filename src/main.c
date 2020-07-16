#include <stdio.h>
#include <stdlib.h>

#include "evaluator.h"
#include "object.h"
#include "reader.h"

static const char SQUEAKY_PROMPT[] = "squeaky> ";
static const char SQUEAKY_HISTORY_FILE[] = ".squeaky_history";

int
main(int argc, char* argv[])
{
    puts("Welcome to Squeaky Scheme!");
    puts("Use Ctrl-c to exit.");

    for (;;) {
        printf(SQUEAKY_PROMPT);

        struct object exp = { 0 };
        int rc = reader_read(stdin, &exp);
        if (rc != READER_OK) break;

        struct object res = { 0 };
        rc = evaluator_eval(&exp, &res);
        if (rc != EVALUATOR_OK) break;

        object_print(&res);
        putchar('\n');
    }
    

    return EXIT_SUCCESS;
}
