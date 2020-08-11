#ifndef SQUEAKY_READER_H_INCLUDED
#define SQUEAKY_READER_H_INCLUDED

#include <stdio.h>

#include "value.h"

struct value* reader_read(FILE* fp);

#endif
