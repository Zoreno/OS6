#include <ctype.h>

int iscntrl(char c)
{
    return c < 0x20 || c == 0x7f;
}