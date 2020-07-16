#ifndef SQUEAKY_READER_H_INCLUDED
#define SQUEAKY_READER_H_INCLUDED

#include <stdio.h>

#include "object.h"

enum reader_status {
    READER_OK = 0,
    READER_ERROR,
};

int reader_read(FILE* stream, struct object* object);

#endif
