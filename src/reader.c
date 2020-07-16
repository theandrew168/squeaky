#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "object.h"
#include "reader.h"

static bool
reader_is_delimiter(int c)
{
    return isspace(c) || c == EOF || c == '(' || c == ')' || c == '"' || c == ';';
}

static int
reader_peek(FILE* stream)
{
    assert(stream != NULL);

    int c = getc(stream);
    ungetc(c, stream);
    return c;
}

static void
reader_skip_whitespace(FILE* stream)
{
    assert(stream != NULL);

    int c = 0;
    while ((c = getc(stream)) != EOF) {
        if (isspace(c)) continue;
        if (c == ';') {  // comments count as whitespace here, too
            while (((c = getc(stream)) != EOF) && (c != '\n'));
            continue;
        }
        ungetc(c, stream);
        break;
    }
}

int
reader_read(FILE* stream, struct object* object)
{
    assert(stream != NULL);
    assert(object != NULL);

    reader_skip_whitespace(stream);
    int c = getc(stream);

    // check for a fixnum
    if (isdigit(c) || (c == '-' && isdigit(reader_peek(stream)))) {
        int sign = 1;
        long num = 0;

        if (c == '-') {
            sign = -1;
        } else {
            ungetc(c, stream);
        }

        while (isdigit(c = getc(stream))) {
            num = (num * 10) + (c - '0');
        }
        num *= sign;

        if (!reader_is_delimiter(c)) {
            fprintf(stderr, "fixnum not followed by delimiter\n");
            return READER_ERROR;
        }

        ungetc(c, stream);
        object_make_fixnum(object, num);
    } else {
        fprintf(stderr, "bad input: unexpected '%c'\n", c);
        return READER_ERROR;
    }

    return READER_OK;
}
