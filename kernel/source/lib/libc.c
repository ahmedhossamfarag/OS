#include "libc.h"

void mem_copy(char *from, char *to, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++)
    {
        *to = *from;
        from++;
        to++;
    }
}
