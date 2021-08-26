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

char tolower(char s1)
{
    if (s1 >= 65 && s1 <= 90)
    {
        s1 += 32;
    }
    return s1;
}

bool is_digit(char c)
{
    return (c >= 48 && c <= 57);
}

int to_numeric_digit(char c)
{
    return c - 48;
}

int strnlen_terminator(const char *str, int max, char terminator)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        if (str[i] == '\0' || str[i] == terminator)
            break;
    }
    return i;
}

int istrncmp(const char *str1, const char *str2, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (str1[i] != str2[i] && tolower(str1[i]) != tolower(str2[i]))
            return str1[i] - str2[i];
        if (str1[i] == '\0')
            return 0;
    }
    return 0;
}

int strncmp(const char *str1, const char *str2, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (str1[i] != str2[i])
            return str1[i] - str2[i];
        if (str1[i] == '\0')
            return 0;
    }
    return 0;
}

char *strcpy(char *dest, const char *src)
{
    char *res = dest;
    while (*src != 0)
    {
        *dest = *src;
        src += 1;
        dest += 1;
    }
    *dest = 0x00;
    return res;
}