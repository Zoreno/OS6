#include <ctype.h>

int isprint(char c)
{
    return c >= ' ' && c <= '~';
}