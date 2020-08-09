#include <util/json.h>

#include <stdio.h>

const char *json_version(void)
{
    static char version[15];
    sprintf(version, "%i.%i.%i", 0, 0, 1);

    return version;
}