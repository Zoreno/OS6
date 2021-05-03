/**
 * @file hexdump.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 *
 * @brief
 *
 * @copyright Copyright (C) 2019,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include <util/hexdump.h>

#include <ctype.h>
#include <stdio.h>

void hexdump(uint8_t *mem, size_t size)
{
    size_t bytes = size;
    const size_t bytes_per_col = 16;

    uint32_t nrows = (bytes + bytes_per_col - 1) / bytes_per_col;

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

//=============================================================================
// End of file
//=============================================================================
