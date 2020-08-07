#include <string.h>

char *index(const char *s, int c)
{
    return strrchr(s, c);
}
