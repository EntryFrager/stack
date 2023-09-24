#include "stack.h"

static const char *err_msgs_arr[] = {
    "NO ERROR.\n",
    "ERROR: null pointer to stack.\n",
    "ERROR: null pointer to stack.data.\n",
    "ERROR: wrong stack size.\n",
    "ERROR: wrong stack position.\n",
    "ERROR: error when opening file.\n",
    "ERROR: error when closing file.\n",
    "ERROR: error when checking left canary\n",
    "ERROR: error when checking right canary\n"
};

int stack_ctor (STACK *stk, size_t capacity)
{
    stk->fp_err = fopen (stk->fp_err_name, "a");

    stk->data = (Element *) calloc (capacity + 2, sizeof (Element));
    memset (stk->data + 1, 0, capacity * sizeof(Element));

    stk->data[0] = CANARY;
    stk->data[capacity + 1] = CANARY;
    stk->data++;

    stk->size = capacity;
    stk->position++;

    assert_stack (stk);

    return 0;
}

int stack_dtor (STACK *stk)
{
    stk->position = -1;
    assert_stack (stk);

    free (stk->data);
    
    stk->data = NULL;
    stk->size = -1;
    stk->position = -1;

    return 0;
}

int stack_push (STACK *stk, Element value)
{
    assert_stack (stk);

    if (stk->size == stk->position)
    {
        stack_realloc (stk);
    }

    stk->data[stk->position++] = value;

    return 0;
}

int stack_pop (STACK *stk)
{
    assert_stack (stk);

    Element elem_pop = stk->data[--stk->position];

    stack_resize (stk);

    return elem_pop;
}

int stack_realloc (STACK *stk)
{
    assert_stack (stk);

    stk->data = (Element *) realloc (stk->data - 1, (stk->size * 2 + 2) * sizeof(int));
    stk->data++;

    memset (stk->data + stk->size, 0, stk->size * sizeof(Element));
    stk->size *= 2;
    stk->data[stk->size] = CANARY;
    
    return 1;
}

int stack_resize (STACK *stk)
{
    assert_stack (stk);

    if (stk->size > stk->position)
    {
        stk->data = (Element *) realloc (stk->data - 1, (stk->position + 1) * sizeof(int));
        stk->data++;
    }

    stk->data[stk->position] = CANARY;
    stk->size = stk->position;
    
    return 1;
}

int stack_verification (STACK *stk)
{
    if (stk == NULL)
    {
        return STACK_ERR;
    }

    if (stk->data == NULL)
    {
        return STACK_DATA_ERR;
    }

    if (stk->position < 0)
    {
        return STACK_POSITION_ERR;
    }

    if (stk->size <= 0)
    {
        return STACK_SIZE_ERR;
    }

    if (stk->fp_err == NULL)
    {
        return STACK_FILE_OPEN_ERR;
    }

    if (stk->data[-1] != CANARY)
    {
        return STACK_LEFT_CANARY_ERR;
    }

    if (stk->data[stk->size] != CANARY)
    {
        return STACK_RIGHT_CANARY_ERR;
    }

    return STACK_OK;
}

int stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err)
{
    if (code_error < ERROR_CNT)
    {
        fprintf (stk->fp_err, "%s\n", err_msgs_arr[code_error]);
    }
    else
    {
        fprintf (stk->fp_err, "Unknown error.\n\n");
    }

    fprintf (stk->fp_err, "stack[%p] \"stk\" called from %s(%d) %s\n", stk, file_err, line_err, func_err);
    fprintf (stk->fp_err, "{\n");
    fprintf (stk->fp_err, "\tposition = %d\n", stk->position);
    fprintf (stk->fp_err, "\tsize = %d\n", stk->size);
    fprintf (stk->fp_err, "\tdata[%p]\n", stk->data);
    fprintf (stk->fp_err, "\t{\n");

    fprintf (stk->fp_err, "\t\tleft canary = %d\n", stk->data[-1]);

    if (stk->size > 0)
    {
        for (int i = 0; i < stk->size; i++)
        {
            fprintf (stk->fp_err, "\t\t*[%d] = %d\n", i, stk->data[i]);
        }
    }

    fprintf (stk->fp_err, "\t\tright canary = %d\n", stk->data[stk->size]);
    
    fprintf (stk->fp_err, "\t}\n");
    fprintf (stk->fp_err, "}\n");

    if (fclose (stk->fp_err) != 0)
    {
        fprintf (stk->fp_err, "ERROR: an error occurred while closing the file.\n");
    }

    return 0;
}