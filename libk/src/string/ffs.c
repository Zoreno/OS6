#include <string.h>

int ffs(int i)
{
    return (__builtin_ffs(i));
}