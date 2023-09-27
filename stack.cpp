#include "stack.h"

static const char *err_msgs_arr[] = {
    "NO ERROR.\n",
    "ERROR: null pointer to stack.\n",
    "ERROR: null pointer to stack data.\n",
    "ERROR: wrong stack size.\n",
    "ERROR: wrong stack position.\n",
    "ERROR: stack size is less than position.\n",
    "ERROR: error when opening file.\n",
    "ERROR: error when closing file.\n",
#ifdef DEBUG_CHECK
    "ERROR: hash does not match true value.\n"
    "ERROR: error when checking stack canary.\n"
    "ERROR: error when checking stack left_canary\n",
    "ERROR: error when checking stack right_canary\n",
    "ERROR: error when checking stack data->canary\n",
    "ERROR: error when checking stack data->left_canary\n",
    "ERROR: error when checking stack data->right_canary\n"
#endif
};

#ifdef DEBUG_CHECK
static const char *gnt_polynom = "00000100110000010001110110110111";
static const size_t polynom_degree = 32;
static const size_t len_message = 512;
#endif

STACK stack_ctor (STACK *stk, size_t capacity)
{
    stk->fp_err = fopen (stk->fp_err_name, "a");

#ifdef DEBUG_CHECK
    stk->data = (ELEMENT *) calloc (capacity * sizeof (ELEMENT) + 2 * sizeof (CANARY_TYPE), sizeof (char));
    my_assert (stk->data != NULL)

    stk->data[0] = CANARY;
    stk->data[capacity + 1] = CANARY;
    stk->data++;

    stk->left_canary = CANARY;
    stk->right_canary = CANARY;
#else
    stk->data = (ELEMENT *) calloc  (capacity * sizeof (ELEMENT), sizeof (ELEMENT));
    my_assert (stk->data != NULL);
#endif

    memset (stk->data, 0, capacity * sizeof (ELEMENT));

    stk->size = capacity;
    stk->position = 0;

#ifdef DEBUG_CHECK
    stk->hash = hash_control (stk);
#endif

    assert_stack (stk);

    return *stk;
}

STACK stack_dtor (STACK *stk)
{
    assert_stack (stk);

    free (stk->data);

#ifdef DEBUG_CHECK
    stk->left_canary = STACK_VALUE_DEFAULT;
    stk->right_canary = STACK_VALUE_DEFAULT;
    stk->hash = STACK_VALUE_DEFAULT;
#endif

    stk->fp_err = NULL;

    stk->data = NULL;
    stk->size = STACK_VALUE_DEFAULT;
    stk->position = STACK_VALUE_DEFAULT;

    return *stk;
}

STACK stack_push (STACK *stk, ELEMENT value)
{
    assert_stack (stk);

    if (stk->size == stk->position)
    {
        stack_realloc (stk, stk->size * 2);
    }

    stk->data[stk->position++] = value;

#ifdef DEBUG_CHECK
    stk->hash = hash_control (stk);
#endif

    assert_stack (stk);

    return *stk;
}

ELEMENT stack_pop (STACK *stk)
{
    assert_stack (stk);

    if (stk->position < stk->size / 3)
    {
        stack_realloc (stk, stk->position);
    }

    ELEMENT elem_pop = stk->data[--stk->position];

    stk->data[stk->position] = 0;

#ifdef DEBUG_CHECK
    stk->hash = hash_control (stk);
#endif

    assert_stack (stk);

    return elem_pop;
}

STACK stack_realloc (STACK *stk, int size)
{
    assert_stack (stk);

#ifdef DEBUG_CHECK
    stk->data = (ELEMENT *) realloc (stk->data - 1, size * sizeof (ELEMENT) + 2 * sizeof (CANARY_TYPE));
    my_assert (stk->data != NULL);

    stk->data++;

    stk->data[size] = CANARY;
#else
    stk->data = (ELEMENT *) realloc (stk->data, size * sizeof (ELEMENT));
    my_assert (stk->data != NULL);
#endif

    if (size - stk->size > 0)
    {
        memset (stk->data + stk->size, 0, (size - stk->size) * sizeof (ELEMENT));
    }

    stk->size = size;

#ifdef DEBUG_CHECK
    stk->hash = hash_control (stk);
#endif

    assert_stack (stk);
    
    return *stk;
}

#ifdef DEBUG_CHECK

HASH_TYPE hash_control (STACK *stk)
{
    my_assert (stk != NULL);
    
    char *msg = (char *) calloc (len_message, sizeof (char));
    my_assert (msg != NULL);
    
    msg = convert_bin_msg (stk, msg);
    
    HASH_TYPE hash = hash_crc_32 (msg);

    free (msg);
    msg = NULL;

    return hash;
}

char *convert_bin_msg (STACK *stk, char *msg)
{
    my_assert (stk != NULL);
    my_assert (msg != NULL);

    size_t len = 0;
    len = conversion_bin_sys (stk->position, msg, len);
    len = conversion_bin_sys (stk->size, msg, len);

    for (int i = 0; i < stk->position; i++)
    {
        len = conversion_bin_sys (stk->data[i], msg, len);
    }

    return msg;
}

int conversion_bin_sys (int value, char *text, size_t text_len)
{
    my_assert (text != NULL);
    
    int bin = 0;
    int x = 1;
    size_t len = 0;

    while (value > 0)
    {
        bin += x * (value % 2);
        x *= 10;
        value /= 2;
        len++;
    }

    for (size_t i = 0; i < len; i++)
    {
        text[i + text_len] = (char) (bin % 10 + 48);
        bin /= 10;
    }

    char temp = '0';

    for (size_t i = 0; i < len / 2; i++)
    {
        temp = text[i + text_len];
        text[i + text_len] = text[len - 1 - i + text_len];
        text[len - 1 - i + text_len] = temp;
    }

    text_len += len;

    return text_len;
}

HASH_TYPE hash_crc_32 (char *message)
{
    my_assert (message != NULL);

    char *reg = (char *) calloc (polynom_degree + 1, sizeof (char));
    my_assert (reg != NULL);

    reg[polynom_degree] = '\0';
    memset (reg, '0', polynom_degree * sizeof (char));

    size_t len_mes = strlen (message);

    for (size_t i = 0; i < len_mes; i++)
    {
        int extend_bit = reg[0];
        for (int j = 0; j < polynom_degree - 1; j++)
        {
            reg[j] = reg[j + 1];
        }

        reg[polynom_degree - 1] = message[i];

        if (extend_bit == '1')
        {
            for (int j = 0; j < polynom_degree; j++)
            {
                if (gnt_polynom[j] == '1')
                {
                    if (reg[j] == '1')
                    {
                        reg[j] = '0';
                    }
                    else
                    {
                        reg[j] = '1';
                    }
                }
            }
        }
    }

    HASH_TYPE hash = 0;
    int degree = polynom_degree;

    for (int i = 0; i < polynom_degree; i++)
    {
        hash += (HASH_TYPE) (reg[i] * pow(10, degree));
        degree--;
    }

    return hash;
}
#endif

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

#ifdef DEBUG_CHECK
    if (stk->hash != hash_control (stk))
    {
        return STACK_HASH_ERR;
    }
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

    if (stk->size < stk->position)
    {
        return STACK_SIZE_POS_ERR;
    }

#ifdef DEBUG_CHECK
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

void stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err)
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

#ifdef DEBUG_CHECK
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

#ifdef DEBUG_CHECK
                fprintf (stk->fp_err, "\t\tdata->left_canary = %x\n", stk->data[-1]);
#endif

                if (stk->size > 0)
                {
                    for (int i = 0; i < stk->size; i++)
                    {
                        fprintf (stk->fp_err, "\t\t*[%d] = %d\n", i, stk->data[i]);
                    }
                }

#ifdef DEBUG_CHECK
                fprintf (stk->fp_err, "\t\tdata->right_canary = %x\n", stk->data[stk->size]);
#endif

            }
            else 
            {
                fprintf (stk->fp_err, "\tdata[NULL]\n");
            }

            fprintf (stk->fp_err, "\t}\n");

#ifdef DEBUG_CHECK
            fprintf (stk->fp_err, "\tstack->hash = %llu\n", stk->hash);
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