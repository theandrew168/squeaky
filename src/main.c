#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "lexer.h"
#include "parser.h"

static char*
file_read(const char* path)
{
    FILE* file = fopen(path, "rb");
    assert(file != NULL);

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(size + 1);
    assert(buffer != NULL);
    long read = fread(buffer, sizeof(char), size, file);
    assert(read == size);
    buffer[read] = '\0';

    fclose(file);
    return buffer;
}

int
main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("Welcome to Squeaky Scheme!\n");
        printf("Use Ctrl-c to exit.\n");

        printf("squeaky> ");

        char line[512] = { 0 };
        while (fgets(line, sizeof(line), stdin) != NULL) {
            // throw the line to the lexer
            struct lexer lexer = { 0 };
            lexer_init(&lexer, line);

            // print out each token
            for (;;) {
                struct token token = lexer_next_token(&lexer);
                if (token.type == TOKEN_EOF) break;

                lexer_token_println(&token);
            }

            // reinit the lexer
            lexer_init(&lexer, line);

            // parse and print the token stream
            struct parser parser = { 0 };
            parser_init(&parser, &lexer);
            struct value* ast = parser_parse(&parser);
            value_println(ast);

            printf("squeaky> ");
        }

        printf("\n");
    } else if (argc == 2) {
        // TODO: handle source files
        char* source = file_read(argv[1]);
        free(source);
    } else {
        fprintf(stderr, "usage: %s [path]\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
