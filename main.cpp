#include "stack.h"

int main ()
{
    STACK stack = {};

    stack_ctor (&stack, 5);

    assert_stack (&stack);

    for (int i = 0; i < 7; i++)
    {
        stack_push (&stack, (Element) i);
    }

    stack.data[-1] = 0;

    assert_stack (&stack);

    for (int i = 0; i < 7; i++)
    {
        Element stack_elem = stack_pop(&stack);

        printf ("%d\n", stack_elem);
    }

    assert_stack (&stack);

    stack_dtor (&stack);

    return 0;
}