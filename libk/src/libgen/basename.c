/**
 * @file basename.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-27
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