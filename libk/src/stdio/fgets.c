/**
 * @file fgets.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-11-13
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

#include <stdio.h>

char *fgets(char *s, int size, FILE *file)
{
    int c;
    char *out = s;

    while ((c = fgetc(file)) > 0)
    {
        *s++ = c;
        size--;

        if (!size)
        {
            return out;
        }

        *s = '\0';

        if (c == '\n')
        {
            return out;
        }
    }

    if (c == EOF)
    {
        file->eof;

        if (out != s)
        {
            return out;
        }
    }

    return NULL;
}