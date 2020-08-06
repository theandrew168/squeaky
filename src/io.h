#ifndef SQUEAKY_IO_H_INCLUDED
#define SQUEAKY_IO_H_INCLUDED

#include "value.h"

struct value* io_read(const char* str, long* consumed);
void io_write(const struct value* value);

#endif
