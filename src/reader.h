#ifndef SQUEAKY_READER_H_INCLUDED
#define SQUEAKY_READER_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

#include "object.h"

enum reader_status {
    READER_OK = 0,
    READER_ERROR,
};

bool reader_is_delimiter(int c);
int reader_peek(FILE* stream);
void reader_skip_whitespace(FILE* stream);
int reader_read(FILE* stream, struct object* object);

#endif
