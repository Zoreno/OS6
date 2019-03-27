#include <stdio.h>

FILE *stdout = NULL;

void set_stdout(FILE *file)
{
    stdout = file;
}