#include <math.h>

float log10f(float a)
{
    float A = 0.3010299956639812f;

    return A * log2f(a);
}