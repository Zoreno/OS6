#include <ctype.h>

char tolower(char c)
{
    return isupper(c) ? c + 0x20 : c;
}