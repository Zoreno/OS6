/**
 * @file eh_framelist.h
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

#ifndef _LIBUNWIND_EH_FRAMELIST_H
#define _LIBUNWIND_EH_FRAMELIST_H

#include <stdint.h>

struct eh_frame_t
{
    void *addr;
    uint64_t size;
};

extern "C" struct eh_frame_t *get_eh_frame_list() noexcept;

#endif

//=============================================================================
// End of file
//=============================================================================