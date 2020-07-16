#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "evaluator.h"
#include "object.h"

int
evaluator_eval(const struct object* exp, struct object* res)
{
    memmove(res, exp, sizeof(struct object));
    return EVALUATOR_OK;
}
