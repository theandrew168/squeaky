.POSIX:
.SUFFIXES:

AR       = ar
CC       = cc
CFLAGS   = -std=c99
CFLAGS  += -DDEBUG_TRACE_EXECUTION
CFLAGS  += -fPIC -g -Og
CFLAGS  += -Wall -Wextra -Wpedantic
CFLAGS  += -Wno-unused-parameter -Wno-unused-result -Wno-unused-function
CFLAGS  += -Isrc/
LDFLAGS  =
LDLIBS   = -lGL -lSDL2

default: squeaky
all: libsqueaky.a libsqueaky.so squeaky

libsqueaky_sources =  \
  src/env.c           \
  src/lexer.c         \
  src/parser.c        \
  src/value.c
libsqueaky_objects = $(libsqueaky_sources:.c=.o)

src/chunk.o: src/chunk.c src/chunk.h src/value.h
src/env.o: src/env.c src/env.h src/value.h
src/lexer.o: src/lexer.c src/lexer.h
src/parser.o: src/parser.c src/parser.h src/chunk.h src/lexer.h
src/value.o: src/value.c src/value.h
src/vm.o: src/vm.c src/vm.h src/chunk.h src/lexer.h src/value.h

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
