#include <ctype.h>

int isalpha(char c)
{
    return isupper(c) || islower(c);
}