/// @file error.cpp

#include "error.h"

/**
 * types of errors
*/

static const char* err_msgs_arr[] = {
    "������ ���.\n",
    "ERROR: ��������� ������ ��� �������� �����.\n",
    "ERROR: ��������� ������ ��� �������� �����.\n"
    "ERROR: ��������� ������ ��� ���������� ������ �� �����.\n"
};

/**
 * Error return function.
 * @param[in] code_error
*/

const char* my_strerr (unsigned code_error)
{
    if (code_error < ERROR_CNT)
    {
        return err_msgs_arr[code_error];
    }
    else
    {
        return "����������� ������.\n";
    }
}
