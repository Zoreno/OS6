/**
 * @file fegetround.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-26
 * 
 * @brief Get the current rounding mode
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

#include <fenv.h>

#include <stdint.h>

// TODO: This shouldn't be linked this way
extern uint16_t arch_x86_64_get_rounding_mode();
extern void arch_x86_64_set_rounding_mode(uint16_t mode);

int fesetround(int mode)
{
    uint16_t old = arch_x86_64_get_rounding_mode();

    arch_x86_64_set_rounding_mode(mode);

    return old;
}