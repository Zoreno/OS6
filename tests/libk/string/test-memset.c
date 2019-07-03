#include <string.h>

#include "../../../libk/src/string/memset.c"

#include <jtest.h>

JTEST(memset, test)
{
	char data[100];

	libk_memset(data, 0, 100);

	for (int i = 0; i < 100; ++i)
	{
		ASSERT_EQUAL(data[i], 0);
	}
}