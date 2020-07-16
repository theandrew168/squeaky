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

static int
reader_skip_expected_string(FILE* stream, const char* str)
{
    while (*str != '\0') {
        int c = getc(stream);
        if (c != *str) {
            fprintf(stderr, "unexpected character: '%c'\n", c);
            return READER_ERROR;
        }
        str++;
    }

    return READER_OK;
}

static int
reader_peek_expected_delimiter(FILE* stream)
{
    if (!reader_is_delimiter(reader_peek(stream))) {
        fprintf(stderr, "character literal not followed by delimiter\n");
        return READER_ERROR;
    }

    return READER_OK;
}

//static int reader_read_boolean(FILE* stream, struct object* object);
//static int reader_read_fixnum(FILE* stream, struct object* object);

static int
reader_read_character(FILE* stream, struct object* object)
{
    int c = getc(stream);
    switch (c) {
        case EOF:
            fprintf(stderr, "incomplete character literal\n");
            return READER_ERROR;
        case 's':
            if (reader_peek(stream) == 'p') {
                int rc = reader_skip_expected_string(stream, "pace");
                if (rc != READER_OK) return rc;

                rc = reader_peek_expected_delimiter(stream);
                if (rc != READER_OK) return rc;

                object_make_character(object, ' ');
                return READER_OK;
            }
            break;
        case 'n':
            if (reader_peek(stream) == 'e') {
                int rc = reader_skip_expected_string(stream, "ewline");
                if (rc != READER_OK) return rc;

                rc = reader_peek_expected_delimiter(stream);
                if (rc != READER_OK) return rc;

                object_make_character(object, '\n');
                return READER_OK;
            }
            break;
    }

    reader_peek_expected_delimiter(stream);
    object_make_character(object, c);
    return READER_OK;
}

//static int reader_read_string(FILE* stream, struct object* object);

int
reader_read(FILE* stream, struct object* object)
{
    assert(stream != NULL);
    assert(object != NULL);

    reader_skip_whitespace(stream);
    int c = getc(stream);

    if (c == '#') {  // check for boolean
        c = getc(stream);
        switch (c) {
            case 't':
                object_make_boolean(object, true);
                break;
            case 'f':
                object_make_boolean(object, false);
                break;
            case '\\':
                return reader_read_character(stream, object);
            default:
                fprintf(stderr, "unknown boolean or character literal\n");
                return READER_ERROR;
        }
    } else if (isdigit(c) || (c == '-' && isdigit(reader_peek(stream)))) {  // check for fixnum
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
