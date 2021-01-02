/**
 * @file memory_info.h
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

#ifndef _MEMORY_INFO_H
#define _MEMORY_INFO_H

#include <stdint.h>

typedef struct
{
    uint64_t addr;
    uint64_t len;
} mem_region_t;

#define MEM_INFO_MAX_REGIONS 16

typedef struct
{
    uint64_t memory_size;

    mem_region_t regions[MEM_INFO_MAX_REGIONS];

    uint64_t kernel_load_addr;
    uint64_t kernel_size;
    uint64_t kernel_end;

} memory_info_t;

#endif

//=============================================================================
// End of file
//=============================================================================