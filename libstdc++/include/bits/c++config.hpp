/**
 * @file c++config.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-07-28
 * 
 * @brief C++ standard library configuration
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

#ifndef __CXX_CONFIG_HPP
#define __CXX_CONFIG_HPP

#ifndef OS6STD
/**
 * Defines which namespace to place the library in. Can be redefined for
 * for example unit testing.
 */
#define OS6STD std
#endif

#define OS6STD_BEGIN_SYSTEM_HEADER \
    extern "C++"                   \
    {

#define OS6STD_END_SYSTEM_HEADER \
    }

namespace OS6STD
{

// TODO: This should be dynamically determined
using size_t = unsigned long;
using ptrdiff_t = signed long int;
using nullptr_t = decltype(nullptr);

} // namespace OS6STD

#endif

//=============================================================================
// End of file
//=============================================================================
