#include <ctype.h>

// #include "../../../libk/src/ctype/isalnum.c"

//#undef isalnum

int main(int argc, char **argv)
{
    return isalnum('c') == 0;
}