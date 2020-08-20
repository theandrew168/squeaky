#ifndef SQUEAKY_READER_H_INCLUDED
#define SQUEAKY_READER_H_INCLUDED

#include <stdio.h>

#include "value.h"
#include "vm.h"

struct value* reader_read(struct vm* vm, FILE* fp);

#endif
