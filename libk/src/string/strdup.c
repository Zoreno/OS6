#include <string.h>
#include <stdlib.h>

char *strdup(const char *str)
{
    char *ret = malloc(strlen(str) + 1);

    if (ret)
    {
        strcpy(ret, str);
    }

    return ret;
}