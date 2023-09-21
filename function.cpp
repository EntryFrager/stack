#include "function.h"

int stack_push (STACK *stk, Element value)
{
    my_assert (stk != NULL);
    my_assert (stk->data != NULL);

    if (stk->size <= stk->position)
    {
        stack_realloc (stk);
    }

    stk->data[++stk->position] = value;
    //printf("%d\n", stk->data[stk->position]);

    return 0;
}

int stack_pop (STACK *stk)
{
    my_assert (stk != NULL);
    my_assert (stk->data != NULL);

    Element elem_pop = stk->data[stk->position--];

    return elem_pop;
}

int stack_realloc (STACK *stk)
{
    my_assert (stk != NULL);
    my_assert (stk->data != NULL);

    stk->data = (Element *) realloc (stk->data, stk->size * 2);

    stk->size *= 2;

    return 1;
}