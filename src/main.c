#include <stdio.h>
#include <stdlib.h>

#include "linenoise.h"

static const char LISPY_PROMPT[] = "lispy> ";
static const char LISPY_HISTORY_FILE[] = ".lispy_history";

int
main(int argc, char* argv[])
{
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

        printf("You wrote: %s\n", line);

        linenoiseFree(line);
    }

    return EXIT_SUCCESS;
}
