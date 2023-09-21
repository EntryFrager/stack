#include "stack_ctor_dtor.h"
#include "function.h"

int main ()
{
    STACK stack = {};

    stack_ctor (&stack, 5);

    for (size_t i = 0; i < 7; i++)
    {
        stack_push (&stack, (Element) i);
    }

    for (size_t i = 0; i < 7; i++)
    {
        printf("%d\n", stack_pop (&stack));
    }    

    stack_dtor (&stack);

    return 0;
}