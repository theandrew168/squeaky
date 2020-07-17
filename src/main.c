#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "linenoise.h"
#include "parser.h"

static const char SQUEAKY_PROMPT[] = "squeaky> ";
static const char SQUEAKY_HISTORY_FILE[] = ".squeaky_history";

struct tbuf {
    struct token* buf;
    long count;
};

void
tbuf_append(struct tbuf* tbuf, const struct token token)
{
    struct token* new = realloc(tbuf->buf, (tbuf->count + 1) * sizeof(struct token));
    memmove(&new[tbuf->count], &token, sizeof(struct token));
    tbuf->buf = new;
    tbuf->count++;
}

void
tbuf_free(struct tbuf* tbuf)
{
    free(tbuf->buf);
}

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

//        puts(line);

        struct tbuf tokens = { 0 };

        struct lexer l = {
            .input = line,
            .input_len = strlen(line),
        };

        int rc = LEXER_OK;
        struct token t = { 0 };
        while ((rc = lexer_lex(&l, &t)) == LEXER_OK) {
            tbuf_append(&tokens, t);
        }

        if (rc == LEXER_ERROR) {
            fprintf(stderr, "!!! lexer error !!!\n");
            tbuf_free(&tokens);
            linenoiseFree(line);
            continue;
        }

        rc = parser_parse(tokens.buf, tokens.count);
        if (rc != PARSER_OK) {
            fprintf(stderr, "!!! parser error !!!\n");
            tbuf_free(&tokens);
            linenoiseFree(line);
            continue;
        }

        tbuf_free(&tokens);
        linenoiseFree(line);
    }

    return EXIT_SUCCESS;
}
