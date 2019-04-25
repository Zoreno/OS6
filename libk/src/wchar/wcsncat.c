#include <wchar.h>

wchar_t *wcsncat(wchar_t *dest, const wchar_t *src, size_t n)
{
    wchar_t *end = dest;

    size_t c = 0;

    // read past the first string
    while (*end != 0)
    {
        ++end;
    }

    // Copy source to dest + strlen(dest)
    while (*src && c < n)
    {
        *end = *src;
        ++end;
        ++src;
        ++c;
    }

    // Null terminate
    *end = 0;

    return dest;
}
