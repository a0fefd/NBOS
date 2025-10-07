#include "include/string.h"

size_t strlen(const char* str)
{
    size_t length = 0;
    while (str[length] != '\0')
        length++;
    return length;
}

void strcpy(char *dest, const char *source, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        // if (i > sizeof(dest) || i > sizeof(source)) { break; }
        dest[i] = source[i];
    }
}