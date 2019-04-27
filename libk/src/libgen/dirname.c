/**
 * @file dirname.c
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
#include <stdlib.h>
#include <string.h>

char *dirname(char *path)
{
    int has_slash = 0;

    char *c = path;

    while (*c)
    {
        if (*c == '/')
        {
            has_slash = 1;
        }

        ++c;
    }

    if (!has_slash)
    {
        return ".";
    }

    --c;

    if (c == path)
    {
        return "/";
    }

    while (*c != '/')
    {
        *c = '\0';

        if (c == path)
        {
            break;
        }

        --c;
    }

    if (c == path)
    {
        if (*c == '/')
        {
            return "/";
        }

        return ".";
    }

    while (*c == '/')
    {
        if (c == path)
        {
            return "/";
        }

        *c = '\0';

        --c;
    }

    return path;
}