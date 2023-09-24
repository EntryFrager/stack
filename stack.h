#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define assert_stack(stack) { \
    int code_error = 0; \
    if ((code_error = stack_verification (stack)) != STACK_OK) \
    {   \
        stack_dump (stack, code_error, __FILE__, __func__, __LINE__); \
        abort (); \
    }   \
}

typedef int Element;

typedef unsigned long long Canary_type;

const Element CANARY = -10;

typedef struct {
    Element *data = NULL;

    const char *fp_err_name = "file_err.txt";
    FILE *fp_err = NULL;

    int size = -1;
    int position = -1;
} STACK;

enum code_error
{
    STACK_OK,
    STACK_ERR,
    STACK_DATA_ERR,
    STACK_SIZE_ERR,
    STACK_POSITION_ERR,
    STACK_FILE_OPEN_ERR,
    STACK_FILE_CLOSE_ERR,
    STACK_LEFT_CANARY_ERR,
    STACK_RIGHT_CANARY_ERR,
};

const int ERROR_CNT = 9;

int stack_ctor (STACK *stk, size_t capacity);

int stack_dtor (STACK *stk);

int stack_push (STACK *stk, Element value);

int stack_pop (STACK *stk);

int stack_realloc (STACK *stk);

int stack_resize (STACK *stk);

int stack_verification (STACK *stk);

int stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err);

#endif // STACK_H