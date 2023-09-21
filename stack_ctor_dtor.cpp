#include "stack_ctor_dtor.h"

int stack_ctor (STACK *stk, size_t capacity)
{
    my_assert (stk != NULL);

    stk->data = (Element *) calloc (capacity * 2, sizeof (Element));

    stk->size = capacity;

    return ERR_NO;
}

int stack_dtor (STACK *stk)
{
    my_assert (stk != NULL);
    my_assert (stk->data != NULL); 

    free (stk->data);
    
    stk->data = NULL;
    stk->size = 0;
    stk->position = 0;

    return 0;
}