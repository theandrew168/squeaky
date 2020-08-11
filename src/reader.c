#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "value.h"

// R5RS 7.1.1: Lexical Structure

static bool
is_whitespace(int c)
{
    return strchr(" \f\n\r\t\v", c) != NULL;
}

static bool
is_letter(int c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z');
}

static bool
is_digit(int c)
{
    return c >= '0' && c <= '9';
}

static bool
is_special_initial(int c)
{
    return strchr("!$%&*/:<=>?^_~", c) != NULL;
}

static bool
is_initial(int c)
{
    return is_letter(c) ||
           is_special_initial(c);
}

static bool
is_special_subsequent(int c)
{
    return strchr("+-.@", c) != NULL;
}

static bool
is_subsequent(int c)
{
    return is_initial(c) ||
           is_digit(c) ||
           is_special_subsequent(c);
}

static bool
is_peculiar_identifier(const char* s)
{
    return *s == '+' ||
           *s == '-' ||
           strncmp("...", s, 3) == 0;
}

static int
peek(FILE* fp)
{
    int c = fgetc(fp);
    ungetc(c, fp);
    return c;
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
    int c;
    while (*s != '\0') {
        c = fgetc(fp);
        if (c != *s) {
            fprintf(stderr, "reader: unexpected character: want '%c', got '%c'\n", *s, c);
            exit(EXIT_FAILURE);
        }
        s++;
    }
}

struct value*
reader_read(FILE* fp)
{
    assert(fp != NULL);

    if (feof(fp)) {
        return value_make_eof();
    }

    return EMPTY_LIST;
}
