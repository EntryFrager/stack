#ifndef STACK_CTOR_DTOR_H
#define STACK_CTOR_DTOR_H

#include <stdio.h>

#include "error.h"

typedef int Element;

typedef struct {
    Element *data = NULL;

    size_t size = 0;
    size_t position = -1;
} STACK;

int stack_ctor (STACK *stk, size_t capacity);

int stack_dtor (STACK *stk);

#endif // STACK_CTOR_DTOR_H