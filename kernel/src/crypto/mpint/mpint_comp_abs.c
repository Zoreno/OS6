/**
 * @file mpint_comp_abs.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-08
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

#include <crypto/mpint/mpint.h>

int mpint_comp_abs(const mpint_t *a, const mpint_t *b)
{
    size_t m;
    size_t n;

    m = mpint_get_length(a);
    n = mpint_get_length(b);

    if (m == 0 && n == 0)
    {
        return 0;
    }
    else if (m > n)
    {
        return 1;
    }
    else if (m < n)
    {
        return -1;
    }

    while (n--)
    {
        if (a->data[n] > b->data[n])
        {
            return 1;
        }
        else if (a->data[n] < b->data[n])
        {
            return -1;
        }
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
