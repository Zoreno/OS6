/**
 * @file get_cursor_location.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-01-25
 *
 * @brief
 *
 * @copyright Copyright (C) 2021,
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

#include <simple_cli/vt_100.h>

void vt_100_get_cursor_location(int *line, int *column)
{
    printf("\033[6n");
    char c;
    char buffer[8];
    int index;

    // Make sure that the first response is an escape char.
    c = getc();

    if (c != 27)
    {
        return;
    }

    // The next char should be a bracket.
    c = getc();

    if (c != '[')
    {
        return;
    }

    // Parse the line response.
    memset(buffer, 0, sizeof(buffer));
    index = 0;

    while (1)
    {
        c = getc();

        if (c == ';')
        {
            break;
        }

        buffer[index++] = c;
    }

    int line_int = atoi(buffer);

    if (line)
    {
        *line = line_int;
    }

    // Parse the column response
    memset(buffer, 0, sizeof(buffer));
    index = 0;

    while (1)
    {
        c = getc();

        if (c == 'R')
        {
            break;
        }

        buffer[index++] = c;
    }

    int column_int = atoi(buffer);

    if (column)
    {
        *column = column_int;
    }
}

//=============================================================================
// End of file
//=============================================================================