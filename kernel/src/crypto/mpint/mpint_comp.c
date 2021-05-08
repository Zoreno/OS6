/**
 * @file mpint_comp.c
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

int mpint_comp(const mpint_t *a, const mpint_t *b)
{
    uint32_t a_len;
    uint32_t b_len;

    a_len = mpint_get_length(a);
    b_len = mpint_get_length(b);

    if (a_len == 0 && b_len == 0)
    {
        return 0;
    }
    else if (a_len > b_len)
    {
        return a->sign;
    }
    else if (a_len < b_len)
    {
        return -b->sign;
    }

    if (a->sign > 0 && b->sign < 0)
    {
        return 1;
    }
    else if (a->sign < 0 && b->sign > 0)
    {
        return -1;
    }

    while (b_len--)
    {
        if (a->data[b_len] > b->data[b_len])
        {
            return a->sign;
        }
        else if (a->data[b_len] < b->data[b_len])
        {
            return -a->sign;
        }
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
