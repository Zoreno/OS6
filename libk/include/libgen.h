/** \addtogroup libk 
 *  @{
 */

/**
 * @file libk/include/libgen.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-25
 * 
 * @brief Extract file name and directory from path
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

#ifndef _LIBK_LIGBEN_H
#define _LIBK_LIBGEN_H

#include <__libk_common.h>

char *basename(char *path);
char *dirname(char *path);

#endif

/** @}*/

//==============================================================================
// End of file
//==============================================================================