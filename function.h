#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>

#include "stack_ctor_dtor.h"
#include "error.h"

int stack_push (STACK *stk, Element value);

int stack_pop (STACK *stk);

int stack_realloc (STACK *stk);

#endif // FUNCTION_H