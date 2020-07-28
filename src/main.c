#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "env.h"
#include "lexer.h"
#include "parser.h"
#include "value.h"

static struct value* eval(struct value* exp, struct env* env);

// SICP 4.1.1
static struct value*
eval(struct value* exp, struct env* env)
{
    if (value_is_self_evaluating(exp)) return exp;
    if (value_is_variable(exp)) return env_get(env, exp->as.symbol);
    if (value_is_definition(exp)) {
        env_def(env, CADR(exp)->as.symbol, CADDR(exp));
        return value_make_symbol("ok", 2);
    }

    fprintf(stderr, "unknown expression type\n");
    return NULL;
}

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
    struct env* env = env_make();

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
            printf("\n");
            printf("Lexer Output\n");
            printf("------------\n");
            for (;;) {
                struct token token = lexer_next_token(&lexer);
                if (token.type == TOKEN_EOF) break;

                lexer_token_println(&token);
            }

            // reinit the lexer
            lexer_init(&lexer, line);

            // parse the token stream
            struct parser parser = { 0 };
            parser_init(&parser, &lexer);
            struct value* exp = parser_parse(&parser);

            // print the resulting AST
            printf("\n");
            printf("Parser Output\n");
            printf("-------------\n");
            value_println(exp);

            // eval the expr
            struct value* res = eval(exp, env);

            // print the result
            printf("\n");
            printf("Result\n");
            printf("------\n");
            value_println(res);

            // print out the current env
            printf("\n");
            printf("Environment\n");
            printf("-----------\n");
            env_print(env);

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

    env_free(env);
    return EXIT_SUCCESS;
}
