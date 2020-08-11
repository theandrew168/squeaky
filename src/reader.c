#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "value.h"

// R5RS 7.1.1: Lexical Structure

#define is_whitespace(c)  \
  (strchr(" \f\n\r\t\v", c) != NULL)
#define is_letter(c)  \
  ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
#define is_digit(c)  \
  (c >= '0' && c <= '9')
#define is_delimiter(c)  \
  (is_whitespace(c) || strchr("()\";", c) != NULL)
#define is_special_initial(c)  \
  (strchr("!$%&*/:<=>?^_~", c) != NULL)
#define is_initial(c)  \
  (is_letter(c) || is_special_initial(c))
#define is_special_subsequent(c)  \
  (strchr("+-.@", c) != NULL)
#define is_subsequent(c)  \
  (is_initial(c) || is_digit(c) || is_special_subsequent(c))
#define is_peculiar_identifier(s)  \
  (*s == '+' || *s == '-' || strncmp("...", s, 3) == 0)

static int
advance(FILE* fp)
{
    // grab the next character
    return fgetc(fp);
}

static int
peek(FILE* fp)
{
    // grab the next character without changing position
    int c = fgetc(fp);
    ungetc(c, fp);
    return c;
}

static void
expect_delimiter(FILE* fp)
{
    // expect a delimiter to follow some token otherwise raise an error
    if (!is_delimiter(peek(fp))) {
        fprintf(stderr, "reader: expected delimiter\n");
        exit(EXIT_FAILURE);
    }
}

static void
eat_whitespace(FILE* fp)
{
    int c;
    while ((c = fgetc(fp)) != EOF) {
        // if whitespace, eat and advance
        if (is_whitespace(c)) continue;

        // if comment found, eat til end of line
        if (c == ';') {
            while (((c = fgetc(fp)) != EOF) && (c != '\n'));
            continue;
        }

        // found a non-whitespace char at this point (put it back and break)
        ungetc(c, fp);
        break;
    }
}

static void
eat_string(FILE* fp, const char* s)
{
    // ensure that an expected string comes next in the file
    while (*s != '\0') {
        int c = fgetc(fp);
        if (c != *s) {
            fprintf(stderr, "reader: unexpected character: want '%c', got '%c'\n", *s, c);
            exit(EXIT_FAILURE);
        }
        s++;
    }
}

struct value*
read_string(FILE* fp)
{
    // skip the leading quote
    advance(fp);
    long start = ftell(fp);
    if (start == -1) {
        perror("failed to capture file position");
        exit(EXIT_FAILURE);
    }

    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '"') break;
    }

    if (feof(fp)) {
        fprintf(stderr, "reader: unterminated string\n");
        exit(EXIT_FAILURE);
    }

    long end = ftell(fp);
    long size = end - start + 1;

    // make a buffer, roll back, fread, copy to value, free?
    // there has to be a cleaner way

    // skip the tailing quote
    advance(fp);
//    return value;
}

struct value*
reader_read(FILE* fp)
{
    assert(fp != NULL);

    eat_whitespace(fp);

    if (feof(fp)) {
        return value_make_eof();
    }

    int c = peek(fp);
    if (c == '"') {
        return read_string(fp);
    }

    fprintf(stderr, "reader: invalid syntax\n");
    exit(EXIT_FAILURE);
}
