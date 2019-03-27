#include <string.h>

char *strcpy(char *str1, const char *str2)
{
    char *dest = str1;

    do
    {
        *str1++ = *str2++;
    } while (*str2);

    *str1 = '\0';

    return dest;
}