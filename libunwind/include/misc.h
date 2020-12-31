/**
 * @file misc.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-31
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

#ifndef _LIBUNWIND_MISC_H
#define _LIBUNWIND_MISC_H

#include <abort.h>
#include <stdint.h>

inline uint64_t bfabs(int64_t value)
{
    return value >= 0 ? static_cast<uint64_t>(value) : static_cast<uint64_t>(-value);
}

inline uint64_t add_offset(uint64_t value, int64_t offset)
{
    auto abs_offset = bfabs(offset);

    if (offset >= 0)
    {
        return value + abs_offset;
    }

    if (value >= abs_offset)
    {
        return value - abs_offset;
    }

    ABORT("attempted add an offset that would result in overflow");
    return 0;
}

#endif

//=============================================================================
// End of file
//=============================================================================