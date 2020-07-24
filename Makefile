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
LDLIBS  = -lGL -lSDL2

default: squeaky
all: libsqueaky.a libsqueaky.so squeaky

libsqueaky_sources =  \
  src/lenv.c          \
  src/lval.c          \
  src/lval_builtin.c  \
  src/lval_error.c    \
  src/lval_lambda.c   \
  src/lval_list.c     \
  src/lval_number.c   \
  src/lval_string.c   \
  src/lval_symbol.c   \
  src/lval_window.c
libsqueaky_objects = $(libsqueaky_sources:.c=.o)

src/lenv.o: src/lenv.c src/lenv.h src/lval.h src/lval_symbol.h
src/lval.o: src/lval.c src/lval.h src/lenv.h
src/lval_builtin.o: src/lval_builtin.c src/lval_builtin.h src/lval.h
src/lval_error.o: src/lval_error.c src/lval_error.h src/lval.h
src/lval_lambda.o: src/lval_lambda.c src/lval_lambda.h src/lval.h src/lenv.h
src/lval_number.o: src/lval_number.c src/lval_number.h src/lval.h
src/lval_list.o: src/lval_list.c src/lval_list.h src/lval.h
src/lval_string.o: src/lval_string.c src/lval_string.h src/lval.h
src/lval_symbol.o: src/lval_symbol.c src/lval_symbol.h src/lval.h
src/lval_window.o: src/lval_window.c src/lval_window.h src/lval.h

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
