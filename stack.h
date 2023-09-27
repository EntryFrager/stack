#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//#define DEBUG_CHECK

#ifdef DEBUG_CHECK
    typedef unsigned long long CANARY_TYPE;

    typedef unsigned long long HASH_TYPE;

    const CANARY_TYPE CANARY = 0xDEADBEEF;
#endif

#define assert_stack(stack) { \
    if ((code_error = stack_verification (stack)) != STACK_OK) \
    {   \
        stack_dump (stack, code_error, __FILE__, __func__, __LINE__); \
        abort (); \
    }   \
}

static int code_error = 0;

#define my_assert(expr) if (!(expr)) {  \
    fprintf(stderr, "%s %s:%d: My assertion failed: \"" #expr "\"\n", __FILE__, __func__, __LINE__); \
    exit(1); \
}

typedef int ELEMENT;

const ELEMENT STACK_VALUE_DEFAULT = -100;

typedef struct {
#ifdef DEBUG_CHECK
    CANARY_TYPE left_canary = CANARY;
#endif

    ELEMENT *data = NULL;

    const char *fp_err_name = "file_err.txt";
    FILE *fp_err = NULL;

    int size = -STACK_VALUE_DEFAULT;
    int position = STACK_VALUE_DEFAULT;

#ifdef DEBUG_CHECK
    HASH_TYPE hash = STACK_VALUE_DEFAULT;
    CANARY_TYPE right_canary = CANARY;
#endif
} STACK;

enum code_error
{
    STACK_OK,
    STACK_ERR,
    STACK_DATA_ERR,
    STACK_SIZE_ERR,
    STACK_POSITION_ERR,
    STACK_SIZE_POS_ERR,
    STACK_FILE_OPEN_ERR,
    STACK_FILE_CLOSE_ERR,

#ifdef DEBUG_CHECK
    STACK_HASH_ERR,
    STACK_CANARY_ERR,   
    STACK_LEFT_CANARY_ERR,
    STACK_RIGHT_CANARY_ERR,
    STACK_DATA_CANARY_ERR,   
    STACK_DATA_LEFT_CANARY_ERR,
    STACK_DATA_RIGHT_CANARY_ERR,
#endif

};

#ifdef DEBUG_CHECK
    const int ERROR_CNT = 14;
#else
    const int ERROR_CNT = 7;
#endif

STACK stack_ctor (STACK *stk, size_t capacity);

STACK stack_dtor (STACK *stk);

STACK stack_push (STACK *stk, ELEMENT value);

ELEMENT stack_pop (STACK *stk);

STACK stack_realloc (STACK *stk, int size);

#ifdef DEBUG_CHECK
HASH_TYPE hash_control (STACK *stk);

char *convert_bin_msg (STACK *stk, char *msg);

int conversion_bin_sys(int value, char *text, size_t text_len);

HASH_TYPE hash_crc_32 (char *message);
#endif

int stack_verification (STACK *stk);

void stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err);

#endif // STACK_H