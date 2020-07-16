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

# https://github.com/antirez/linenoise
# compile linenoise as C99 (plus POSIX.1-2008) with debug symbols
LINENOISE_CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -fPIC -g -Og

default: squeaky
all: libsqueaky.a libsqueaky.so squeaky

libsqueaky_sources =  \
  src/linenoise.c
libsqueaky_objects = $(libsqueaky_sources:.c=.o)

src/linenoise.o: src/linenoise.c src/linenoise.h
	@echo "CC      $@"
	@$(CC) -c $(LINENOISE_CFLAGS) -o $@ $<

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
	./squeaky

.PHONY: clean
clean:
	rm -fr squeaky squeaky_tests *.a *.so src/*.o

.SUFFIXES: .c .o
.c.o:
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c -o $@ $<
