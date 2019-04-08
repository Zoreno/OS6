#include <string.h>

static size_t lfind(const char *str, const char accept)
{
    size_t i = 0;

    while (str[i] != accept)
    {
        ++i;
    }

    return (size_t)(str) + i;
}

char *strtok_r(char *str, const char *delim, char **saveptr)
{
    char *token;

    if (str == NULL)
    {
        str = *saveptr;
    }

    str += strspn(str, delim);

    if (*str == '\0')
    {
        *saveptr = str;
        return NULL;
    }

    token = str;

    str = strpbrk(token, delim);

    if (str == NULL)
    {
        *saveptr = (char *)lfind(token, '\0');
    }
    else
    {
        *str = '\0';
        *saveptr = str + 1;
    }

    return token;
}