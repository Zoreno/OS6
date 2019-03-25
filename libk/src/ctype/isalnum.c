#include <ctype.h>

int isalnum(char c)
{
    return isalpha(c) || isdigit(c);
}