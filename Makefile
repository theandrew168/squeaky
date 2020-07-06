.POSIX:
.SUFFIXES:

AR      = ar
CC      = cc
CFLAGS  = -std=c99
CFLAGS += -fPIC -g -Og
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wno-unused -Wno-unused-result
CFLAGS += -Isrc/
LDFLAGS =
LDLIBS  =

default: lispy
all: liblispy.a lispy

liblispy_sources =
liblispy_objects = $(liblispy_sources:.c=.o)


liblispy.a: $(liblispy_objects)
	$(AR) rcs $@ $(liblispy_objects)

lispy: src/main.c liblispy.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main.c liblispy.a $(LDLIBS)

.PHONY: clean
clean:
	rm -fr lispy lispy_tests *.a *.so src/*.o

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
