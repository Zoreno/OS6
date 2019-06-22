/**
 * @file strspn.c
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

#include <string.h>

size_t strspn(const char *str, const char *accept)
{
    const char *ptr;
    const char *acc;

    size_t size = 0;
    for (ptr = str; *ptr != '\0'; ++ptr)
    {
        for (acc = accept; *acc != '\0'; ++acc)
        {
            if (*ptr == *acc)
            {
                break;
            }
        }

        if (*acc == '\0')
        {
            return size;
        }
        else
        {
            ++size;
        }
    }

    return size;
}