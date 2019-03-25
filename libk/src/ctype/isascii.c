#include <ctype.h>

int isascii(char c)
{
    return ((unsigned char)c) <= 0x7f;
}