/**
 * @file handle_backet.c
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

int vt_100_handle_bracket()
{
    char c = getc();

    switch (c)
    {
    case 'A':
        return VT_100_UP;
    case 'B':
        return VT_100_DOWN;
    case 'C':
        return VT_100_RIGHT;
    case 'D':
        return VT_100_LEFT;
    default:
        break;
    }

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
