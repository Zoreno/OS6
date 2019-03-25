#include <ctype.h>

int isspace(char c)
{
    return c == ' ' || (c >= '\t' && c <= '\r');
}