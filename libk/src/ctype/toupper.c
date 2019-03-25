#include <ctype.h>

char toupper(char c)
{
    return islower(c) ? c - 0x20 : c;
}