#include <ctype.h>

int ispunct(char c)
{
    return isgraph(c) && !(isalnum(c));
}