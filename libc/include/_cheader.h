/** \addtogroup libc 
 *  @{
 */

/**
 * @file libc/include/_cheader.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-25
 * 
 * @brief Prologue and epilogue for all C headers in the c standard library
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

#ifndef _LIBC_CHEADER_H
#define _LIBC_CHEADER_H

#ifdef __cplusplus
#define _c_header_begin \
    extern "C"          \
    {
#define _c_header_end }
#else
#define _c_header_begin
#define _c_header_end
#endif

#endif

/** @}*/

//=============================================================================
// End of file
//=============================================================================