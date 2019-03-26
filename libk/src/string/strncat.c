#include <string.h>
#include <stdint.h>

char *strncat(char *str1, const char *str2, size_t n)
{
    strncpy(str1 + strlen(str1), str2, n);

    return str1;
}