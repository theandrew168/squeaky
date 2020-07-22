.POSIX:
.SUFFIXES:

AR      = ar
CC      = cc
CFLAGS  = -std=c99
CFLAGS += -fPIC -g -Og
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wno-unused-parameter -Wno-unused-result
CFLAGS += -Isrc/
LDFLAGS =
LDLIBS  =

default: squeaky
all: libsqueaky.a libsqueaky.so squeaky

libsqueaky_sources =  \
  src/lenv.c          \
  src/lval.c
libsqueaky_objects = $(libsqueaky_sources:.c=.o)

src/lenv.o: src/lenv.c src/lenv.h src/lval.h
src/lval.o: src/lval.c src/lval.h src/lenv.h

libsqueaky.a: $(libsqueaky_objects)
	@echo "STATIC  $@"
	@$(AR) rcs $@ $(libsqueaky_objects)

libsqueaky.so: $(libsqueaky_objects)
	@echo "SHARED  $@"
	@$(CC) $(LDFLAGS) -shared -o $@ $(libsqueaky_objects) $(LDLIBS)

squeaky: src/main.c libsqueaky.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main.c libsqueaky.a $(LDLIBS)

.PHONY: run
run: squeaky
	rlwrap ./squeaky

.PHONY: clean
clean:
	rm -fr squeaky squeaky_tests *.a *.so src/*.o

.SUFFIXES: .c .o
.c.o:
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c -o $@ $<
