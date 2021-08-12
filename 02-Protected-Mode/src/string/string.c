#include "string.h"

int strlen(const char *ptr)
{
    int len = 0;
    while (*ptr != 0)
    {
        len++;
        ptr += 1;
    }
    return len;
}

int strnlen(const char *ptr, int max)
{
    int len = 0;
    for (len = 0; len < max; len++)
    {
        if (ptr[len] == 0)
            break;
    }
    return len;
}

bool is_digit(char c)
{
    return (c >= 48 && c <= 57);
}

int to_numeric_digit(char c)
{
    return c - 48;
}