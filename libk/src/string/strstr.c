/**
 * @file strstr.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-10
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2020,
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

#include <string.h>

char *strstr(const char *str1, const char *str2)
{
    const char *searchee = str1;
    const char *lookfor = str2;

    if (*searchee == 0)
    {
        if (*lookfor)
        {
            return (char *) NULL;
        }

        return (char *) searchee;
    }

    while(*searchee)
    {
        size_t i = 0;

        while(1)
        {
            if (lookfor[i] == 0)
            {
                return (char *) searchee;
            }

            if (lookfor[i] != searchee[i])
            {
                break;
            }

            ++i;
        }

        ++searchee;
    }

    return (char *)NULL;
}