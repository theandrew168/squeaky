#ifndef SQUEAKY_EVALUATOR_H_INCLUDED
#define SQUEAKY_EVALUATOR_H_INCLUDED

#include "object.h"

enum evaluator_status {
    EVALUATOR_OK = 0,
    EVALUATOR_ERROR,
};

int evaluator_eval(const struct object* exp, struct object* res);

#endif
