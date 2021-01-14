/**
 * @file asinl.cpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-01-14
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

#include <cmath>

namespace OS6TSD
{
long double asin(long double __x)
{
    return __builtin_asinf(__x);
}
}

//=============================================================================
// End of file
//=============================================================================