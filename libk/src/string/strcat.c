#include <string.h>
#include <stdint.h>

char *strcat(char *str1, const char *str2)
{
    strcpy(str1 + strlen(str1), str2);

    return str1;
}
