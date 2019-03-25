#include <ctype.h>

int isxdigit(char c)
{
    return isdigit(c) ||
           (c >= 'A' && c <= 'F') ||
           (c >= 'a' && c <= 'f');
}