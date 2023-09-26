#include "stack.h"

static const char *err_msgs_arr[] = {
    "NO ERROR.\n",
    "ERROR: null pointer to stack.\n",
    "ERROR: null pointer to stack.data.\n",
    "ERROR: wrong stack size.\n",
    "ERROR: wrong stack position.\n",
    "ERROR: error when opening file.\n",
    "ERROR: error when closing file.\n",
#ifdef CANARY_CHECK
    "ERROR: error when checking stack->canary.\n"
    "ERROR: error when checking stack->left_canary\n",
    "ERROR: error when checking stack->right_canary\n",
    "ERROR: error when checking stack->data->canary\n",
    "ERROR: error when checking stack->data->left_canary\n",
    "ERROR: error when checking stack->data->right_canary\n"
#endif
};

STACK stack_ctor (STACK *stk, size_t capacity)
{
    stk->fp_err = fopen (stk->fp_err_name, "a");

#ifdef CANARY_CHECK
    stk->data = (Element *) calloc (capacity * sizeof (Element) + 2 * sizeof (Canary_type), sizeof (char));
    my_assert (stk->data != NULL)

    stk->data[0] = CANARY;
    stk->data[capacity + 1] = CANARY;
    stk->data++;

    stk->left_canary = CANARY;
    stk->right_canary = CANARY;
#else
    stk->data = (Element *) calloc  (capacity * sizeof (Element), sizeof (Element));
    my_assert (stk->data != NULL);
#endif

    memset (stk->data, 0, capacity * sizeof (Element));

    stk->size = capacity;
    stk->position = 0;

    assert_stack (stk);

    return *stk;
}

STACK stack_dtor (STACK *stk)
{
    assert_stack (stk);

    free (stk->data);

#ifdef CANARY_CHECK
    stk->left_canary = STACK_VALUE_DEFAULT;
    stk->right_canary = STACK_VALUE_DEFAULT;
#endif

    stk->fp_err = NULL;

    stk->data = NULL;
    stk->size = STACK_VALUE_DEFAULT;
    stk->position = STACK_VALUE_DEFAULT;

    return *stk;
}

STACK stack_push (STACK *stk, Element value)
{
    assert_stack (stk);

    if (stk->size <= stk->position)
    {
        stack_realloc (stk, stk->size * 2);
    }

    stk->data[stk->position++] = value;

    assert_stack (stk);

    return *stk;
}

Element stack_pop (STACK *stk)
{
    assert_stack (stk);

    Element elem_pop = stk->data[--stk->position];

    if (stk->position < stk->size / 3)
    {
        stack_realloc (stk, stk->position);
    }

    assert_stack (stk);

    return elem_pop;
}

STACK stack_realloc (STACK *stk, int size)
{
    assert_stack (stk);

#ifdef CANARY_CHECK
    stk->data = (Element *) realloc (stk->data - 1, size * sizeof (Element) + 2 * sizeof (Canary_type));
    my_assert (stk->data != NULL);

    stk->data++;

    stk->data[size] = CANARY;
#else
    stk->data = (Element *) realloc (stk->data, size * sizeof (Element));
    my_assert (stk->data != NULL);
#endif

    if (size - stk->size > 0)
    {
        memset (stk->data + stk->size, 0, (size - stk->size) * sizeof (Element));
    }

    stk->size = size;

    assert_stack (stk);
    
    return *stk;
}

int stack_verification (STACK *stk)
{
    if (stk == NULL)
    {
        return STACK_ERR;
    }

    if (stk->fp_err == NULL)
    {
        return STACK_FILE_OPEN_ERR;
    }

#ifdef CANARY_CHECK
    if (stk->left_canary != CANARY && stk->right_canary != CANARY)
    {
        return STACK_CANARY_ERR;
    }

    if (stk->left_canary != CANARY)
    {
        return STACK_LEFT_CANARY_ERR;
    }

    if (stk->right_canary != CANARY)
    {
        return STACK_RIGHT_CANARY_ERR;
    }
#endif

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

#ifdef CANARY_CHECK
    if (stk->data[-1] != (int) CANARY && stk->data[stk->size] != (int) CANARY)
    {
        return STACK_DATA_CANARY_ERR;
    }

    if ( stk->data[-1] != (int) CANARY)
    {
        return STACK_DATA_LEFT_CANARY_ERR;
    }

    if ( stk->data[stk->size] != (int) CANARY)
    {
        return STACK_DATA_RIGHT_CANARY_ERR;
    }
#endif

    return STACK_OK;
}

int stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err)
{
    if (stk != NULL)
    {
        if (code_error != STACK_FILE_OPEN_ERR)
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
#ifdef CANARY_CHECK
            fprintf (stk->fp_err, "\tstack->left_canary = %x\n", stk->left_canary);
#endif
            fprintf (stk->fp_err, "\tfp_err_name = %s\n", stk->fp_err_name);
            fprintf (stk->fp_err, "\tfp_err[%p]\n", stk->fp_err);
            fprintf (stk->fp_err, "\tposition = %d\n", stk->position);
            fprintf (stk->fp_err, "\tsize = %d\n", stk->size);

            if (stk->data != NULL)
            {
                fprintf (stk->fp_err, "\tdata[%p]\n", stk->data);
                fprintf (stk->fp_err, "\t{\n");

#ifdef CANARY_CHECK
                fprintf (stk->fp_err, "\t\tdata->left_canary = %x\n", stk->data[-1]);
#endif

                if (stk->size > 0)
                {
                    for (int i = 0; i < stk->size; i++)
                    {
                        fprintf (stk->fp_err, "\t\t*[%d] = %d\n", i, stk->data[i]);
                    }
                }

#ifdef CANARY_CHECK
                fprintf (stk->fp_err, "\t\tdata->right_canary = %x\n", stk->data[stk->size]);
#endif
            }
            else 
            {
                fprintf (stk->fp_err, "\tdata[NULL]\n");
            }

            fprintf (stk->fp_err, "\t}\n");
#ifdef CANARY_CHECK
            fprintf (stk->fp_err, "\tstack->right_canary = %x\n", stk->right_canary);
#endif
            fprintf (stk->fp_err, "}\n");

            if (fclose (stk->fp_err) != 0)
            {
                fprintf (stk->fp_err, "%s", err_msgs_arr[6]);
            }
        }
        else
        {
            if (code_error < ERROR_CNT)
            {
                fprintf (stderr, "%s\n", err_msgs_arr[code_error]);
            }
            else
            {
                fprintf (stderr, "Unknown error.\n\n");
            }

            fprintf (stderr, "stack[%p] \"stk\" called from %s(%d) %s\n", stk, file_err, line_err, func_err);
            fprintf (stderr, "{\n");
            fprintf (stderr, "\tfp_err_name = %s\n", stk->fp_err_name);
            fprintf (stderr, "\tfp_err[%p]\n", stk->fp_err);
            fprintf (stderr, "}\n");
        }
    }
    else
    {
        if (code_error < ERROR_CNT)
        {
            fprintf (stderr, "%s\n", err_msgs_arr[code_error]);
        }
        else
        {
            fprintf (stderr, "Unknown error.\n\n");
        }

        fprintf (stderr, "stack[NULL] \"stk\" called from %s(%d) %s\n", file_err, line_err, func_err);
    }
}