#include <string.h>
#include <limits.h>

int flsl(long int i)
{
	if (i == 0)
    {
		return 0;
    }

	return (sizeof(i) * CHAR_BIT - __builtin_clzl(i));
}
