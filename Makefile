.POSIX:
.SUFFIXES:

AR       = ar
CC       = cc
CFLAGS   = -std=c99
CFLAGS  += -fPIC -g -Og
CFLAGS  += -Wall -Wextra -Wpedantic
CFLAGS  += -Wno-unused-parameter -Wno-unused-result -Wno-unused-function
CFLAGS  += -Isrc/
LDFLAGS  =
LDLIBS   = -lGL -lSDL2

default: squeaky
all: libsqueaky.a libsqueaky.so squeaky squeaky_tests

libsqueaky_sources =  \
  src/builtin.c       \
  src/env.c           \
  src/io.c            \
  src/list.c          \
  src/mce.c           \
  src/value.c
libsqueaky_objects = $(libsqueaky_sources:.c=.o)

src/builtin.o: src/builtin.c src/builtin.h src/io.h src/value.h
src/env.o: src/env.c src/env.h src/io.h src/value.h
src/io.o: src/io.c src/io.h src/list.h src/value.h
src/list.o: src/list.c src/list.h src/value.h
src/mce.o: src/mce.c src/mce.h src/env.h src/io.h src/list.h src/value.h
src/value.o: src/value.c src/value.h

libsqueaky.a: $(libsqueaky_objects)
	@echo "STATIC  $@"
	@$(AR) rcs $@ $(libsqueaky_objects)

libsqueaky.so: $(libsqueaky_objects)
	@echo "SHARED  $@"
	@$(CC) $(LDFLAGS) -shared -o $@ $(libsqueaky_objects) $(LDLIBS)

squeaky: src/main.c libsqueaky.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main.c libsqueaky.a $(LDLIBS)

squeaky_tests_sources =  \
  src/env_test.c         \
  src/value_test.c

squeaky_tests: $(squeaky_tests_sources) src/main_test.c libsqueaky.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main_test.c libsqueaky.a $(LDLIBS)

.PHONY: check
check: squeaky_tests
	./squeaky_tests

.PHONY: run
run: squeaky
	rlwrap ./squeaky

.PHONY: clean
clean:
	rm -fr squeaky squeaky_tests *.a *.so *.exe *.dll src/*.o

.SUFFIXES: .c .o
.c.o:
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c -o $@ $<
