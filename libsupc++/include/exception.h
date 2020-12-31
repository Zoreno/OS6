/**
 * @file exception.h
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

#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#pragma GCC system_header

extern "C++"
{
    namespace std
    {

    /**
     * @brief Base exception class meant to handle all standard exceptions.
     * 
     * 
     */
    class exception
    {
    public:
        /**
		 * @brief Construct a new exception object
		 * 
		 * 
		 */
        exception() noexcept {}

        /**
		 * @brief Destroy the exception object
		 * 
		 * 
		 */
        virtual ~exception() noexcept;

        /**
		 * @brief Construct a new exception object
		 * 
		 * 
		 */
        exception(const exception &) = default;

        /**
		 * @brief Copies an exception object
		 * 
		 * 
		 * @return exception& 
		 */
        exception &operator=(const exception &) = default;

        /**
		 * @brief Construct a new exception object
		 * 
		 * 
		 */
        exception(exception &&) = default;

        /**
		 * @brief Moves an exception object
		 * 
		 * 
		 * @return exception& 
		 */
        exception &operator=(exception &&) = default;

        /**
		 * @brief Get the description of the cause of the exception.
		 * 
		 * 
		 * @return Pointer to c-string.
		 */
        virtual const char *what() const noexcept;
    };
    } // namespace std
}

#endif

//=============================================================================
// End of file
//=============================================================================