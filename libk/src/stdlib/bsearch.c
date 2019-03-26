#include <stdlib.h>

void *bsearch(
    const void *key,
    const void *base,
    size_t nitems,
    size_t size,
    int (*compar)(const void *, const void *))
{
    void *guess;
    int dir;

    while (nitems > 0)
    {
        guess = (char *)base + size * (nitems / 2);

        dir = compar(key, guess);

        if (!dir)
        {
            return guess;
        }

        else if (nitems == 1)
        {
            break;
        }
        else if (dir < 0)
        {
            nitems /= 2;
        }
        else
        {
            base = guess;
            nitems -= nitems / 2;
        }
    }

    return NULL;
}