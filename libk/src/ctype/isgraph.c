#include <ctype.h>

int isgraph(char c)
{
    return (c - 0x21) < 0x5e;
}