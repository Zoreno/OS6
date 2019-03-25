#include <ctype.h>

char toascii(char c)
{
    return ((unsigned char)c) & 0x7f;
}