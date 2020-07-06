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

# https://github.com/orangeduck/mpc
# compile mpc as C99 with debug symbols
MPC_CFLAGS = -std=c99 -fPIC -g -Og

default: lispy
all: liblispy.a liblispy.so lispy

liblispy_sources =  \
  src/linenoise.c   \
  src/mpc.c
liblispy_objects = $(liblispy_sources:.c=.o)

src/linenoise.o: src/linenoise.c src/linenoise.h
	@echo "CC      $@"
	@$(CC) -c $(LINENOISE_CFLAGS) -o $@ $<
src/mpc.o: src/mpc.c src/mpc.h
	@echo "CC      $@"
	@$(CC) -c $(MPC_CFLAGS) -o $@ $<

liblispy.a: $(liblispy_objects)
	@echo "STATIC  $@"
	@$(AR) rcs $@ $(liblispy_objects)

liblispy.so: $(liblispy_objects)
	@echo "SHARED  $@"
	@$(CC) $(LDFLAGS) -shared -o $@ $(liblispy_objects) $(LDLIBS)

lispy: src/main.c liblispy.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main.c liblispy.a $(LDLIBS)

.PHONY: run
run: lispy
	./lispy

.PHONY: clean
clean:
	rm -fr lispy lispy_tests *.a *.so src/*.o

.SUFFIXES: .c .o
.c.o:
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c -o $@ $<
