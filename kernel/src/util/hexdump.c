#include <util/hexdump.h>

#include <ctype.h>
#include <stdio.h>

void hexdump(uint8_t *mem, size_t size)
{
    size_t bytes = size;
    const size_t bytes_per_col = 16;

    int nrows = (bytes + bytes_per_col - 1) / bytes_per_col;

    int width = 0;

    while (bytes)
    {
        bytes /= 16;
        width++;
    }

    for (uint32_t row = 0; row < nrows; ++row)
    {
        printf("%#0*x | ", width, row * bytes_per_col);

        for (uint32_t col = 0; col < bytes_per_col; ++col)
        {
            uint32_t offset = row * bytes_per_col + col;

            if (offset >= size)
            {
                printf("-- ");
            }
            else
            {
                printf("%02x ", mem[offset]);
            }
        }

        printf("|");

        for (uint32_t col = 0; col < bytes_per_col; ++col)
        {
            uint32_t offset = row * bytes_per_col + col;

            if (offset >= size)
            {
                printf(" ");
            }
            else
            {
                char c = mem[offset];

                if (!isprint(c))
                {
                    c = '.';
                }

                char s[2] = {0};
                s[0] = c;

                printf("%s", s);
            }
        }

        printf("|");

        printf("\n");
    }
}