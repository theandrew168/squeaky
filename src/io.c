#include <assert.h>
#include <string.h>

#include "io.h"
#include "list.h"
#include "value.h"

struct value*
io_read(const char* str, long* consumed)
{
    const char space[] = " \f\n\r\t\v;";
    const char digit[] = "0123456789";
    const char alpha[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "+-!$%&*/:<=>?^_~";

    // needed to capture how many chars this value consumes
    const char* start = str;

    // whitespace / comments
    while (strchr(space, *start) && *start != '\0') {
        if (*start == ';') {
            while (*start != '\n' && *start != '\0') start++;
        }
        start++;
    }

    // EOF
    if (*start == '\0') {
        *consumed = start - str;
        return value_make_error("EOF");
    }

    // quote
    if (*start == '\'') {
        const char* iter = start;
        iter++;  // consume quote char

        struct value* target = io_read(iter, consumed);
        iter += *consumed;
        *consumed = iter - str;  // consumed includes initial quote and sub read
        return list_make(2, value_make_symbol("quote"), target);
    }

    // boolean
    // TODO: character "#\"
    // TODO: vector    "#("
    if (*start == '#') {
        const char* iter = start;
        iter++;
        if (strchr("tf", *iter)) {
            bool boolean = *iter == 't';
            iter++;
            *consumed = iter - str;
            return value_make_boolean(boolean);
        }

        return value_make_error("invalid expression");
    }

    // number
    if (strchr(digit, *start)) {
        char* iter = NULL;
        long number = strtol(start, &iter, 10);
        *consumed = iter - str;
        return value_make_number(number);
    }

    // string
    // TODO: handle escapes
    if (*start == '"') {
        const char* iter = start;
        iter++;  // consume open quote

        while (*iter != '"' && *iter != '\0') iter++;
        if (*iter == '\0') return value_make_error("unterminated string");

        // consume close quote
        iter++;

        *consumed = iter - str;

        // exclude quotes from the string's value
        return value_make_stringn(start + 1, iter - start - 2);
    }

    // symbol
    if (strchr(alpha, *start)) {
        const char* iter = start;
        while (strchr(alpha, *iter) && *iter != '\0') iter++;
        *consumed = iter - str;
        return value_make_symboln(start, iter - start);
    }

    // pair / list / s-expression
    // TODO: this case is ugly, need to read hella book
    if (*start == '(') {
        const char* iter = start;
        iter++;  // skip open paren

        // whitespace / comments (again)
        while (strchr(space, *iter) && *iter != '\0') {
            if (*iter == ';') {
                while (*iter != '\n' && *iter != '\0') iter++;
            }
            iter++;
        }

        struct value* list = EMPTY_LIST;
        while (*iter != ')') {

            // read the next value
            struct value* cell = cons(io_read(iter, consumed), EMPTY_LIST);

            // if first child, replace the list ptr
            if (list == EMPTY_LIST) {
                list = cell;
            } else {  // else cons like normal
                struct value* last = list;
                while (cdr(last) != NULL) last = cdr(last);
                last->as.pair.cdr = cell;
            }

            // advance to end of read value
            iter += *consumed;

            // whitespace / comments (AGAIN)
            while (strchr(space, *iter) && *iter != '\0') {
                if (*iter == ';') {
                    while (*iter != '\n' && *iter != '\0') iter++;
                }
                iter++;
            }
        }

        iter++;  // move past the closing paren
        *consumed = iter - str;

        return list;
    }

    return value_make_error("invalid expression");
}

void
io_write(const struct value* value)
{
    if (value == EMPTY_LIST) {
        printf("'()");
        return;
    }

    switch (value->type) {
        case VALUE_BOOLEAN:
            printf("%s", value->as.boolean ? "#t" : "#f");
            break;
        case VALUE_NUMBER:
            printf("%ld", value->as.number);
            break;
        case VALUE_STRING:
            // TODO: handle escapes
            printf("\"%s\"", value->as.string);
            break;
        case VALUE_SYMBOL:
            printf("%s", value->as.symbol);
            break;
        case VALUE_PAIR: {
            printf("(");
            for (const struct value* iter = value; iter != EMPTY_LIST; iter = cdr(iter)) {
                io_write(car(iter));
                if (cdr(iter) != NULL) printf(" ");
            }
            printf(")");
            break;
        }
        case VALUE_BUILTIN:
            printf("<builtin>");
            break;
        case VALUE_LAMBDA:
            printf("<lambda>");
            break;
        case VALUE_WINDOW:
            printf("<window>");
            break;
        case VALUE_EVENT:
            printf("<event:%d>", value->as.event.type);
            break;
        case VALUE_ERROR:
            printf("error: %s", value->as.error);
            break;
        default:
            printf("<undefined>");
    }
}
