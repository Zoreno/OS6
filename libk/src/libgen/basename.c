#include <libgen.h>

#include <string.h>
#include <stdlib.h>

char *basename(char *path)
{
    char *s = path;
    char *c = NULL;

    do
    {
        int done = 0;

        while (*s == '/')
        {
            *s = '\0';
            s++;
            if (!*s)
            {
                done = 1;
                break;
            }
        }

        if (done)
        {
            break;
        }

        c = s;
        s = strchr(c, '/');
    } while (s);

    if (!c)
    {
        return "/";
    }

    return c;
}