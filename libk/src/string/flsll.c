#include <string.h>
#include <limits.h>

int flsll(long long int i)
{
	if (i == 0)
    {
		return 0;
    }

	return (sizeof(i) * CHAR_BIT - __builtin_clzll(i));
}
