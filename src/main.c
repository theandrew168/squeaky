#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char* argv[])
{
    char input[2048] = { 0 };
    puts("Lispy Version 0.0.1");
    puts("Press Ctrl+c to exit\n");

    for (;;) {
        fputs("lispy> ", stdout);
        fgets(input, sizeof(input), stdin);
        printf("No you're a %s", input);
    }

    return EXIT_SUCCESS;
}
