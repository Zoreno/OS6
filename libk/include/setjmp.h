/**
 * @file setjmp.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-02-13
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

#ifndef _LIBK_SETJMP_H
#define _LIBK_SETJMP_H

typedef long long int jmp_buf[8];

void longjmp(jmp_buf buf, int ret) __attribute__((__noreturn__));
int setjmp(jmp_buf buf);

#endif

//=============================================================================
// End of file
//=============================================================================