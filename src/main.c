#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "chunk.h"
#include "vm.h"

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
    int rc = EXIT_SUCCESS;

    struct vm vm = { 0 };
    vm_init(&vm);

    if (argc == 1) {
        printf("Welcome to Squeaky Scheme!\n");
        printf("Use Ctrl-c to exit.\n");

        printf("squeaky> ");

        char line[512] = { 0 };
        while (fgets(line, sizeof(line), stdin) != NULL) {
            vm_interpret(&vm, line);
            printf("squeaky> ");
        }

        printf("\n");
    } else if (argc == 2) {
        char* source = file_read(argv[1]);
        int res = vm_interpret(&vm, source);
        free(source);

        if (res != VM_OK) rc = EXIT_FAILURE;
    } else {
        fprintf(stderr, "usage: %s [path]\n", argv[0]);
        rc = EXIT_FAILURE;
    }

    vm_free(&vm);
    return rc;
}
