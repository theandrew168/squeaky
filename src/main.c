#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"

static const char SQUEAKY_PROMPT[] = "squeaky> ";
static const char SQUEAKY_HISTORY_FILE[] = ".squeaky_history";

int
main(int argc, char* argv[])
{
    linenoiseHistoryLoad(SQUEAKY_HISTORY_FILE);

    char* line = NULL;
    while ((line = linenoise(SQUEAKY_PROMPT)) != NULL) {
        if (line[0] == '\0') {
            linenoiseFree(line);
            break;
        }

        linenoiseHistoryAdd(line);
        linenoiseHistorySave(SQUEAKY_HISTORY_FILE);

        puts(line);

        linenoiseFree(line);
    }

    return EXIT_SUCCESS;
}
