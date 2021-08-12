#include "memory.h"

void *memset(void *ptr, int c, size_t size)
{
    char *c_ptr = (char *)ptr;
    for (int i = 0; i < size; i++)
    {
        c_ptr[i] = (char)c;
    }
    return ptr;
}

int memcmp(void *s1, void *s2, int count)
{
    char *c1 = s1;
    char *c2 = s2;
    for (int i = 0; i < count; i++)
    {
        if (c1[i] != c2[i])
        {
            return c1[i] < c2[i] ? -1 : 1;
        }
    }
    return 0;
}