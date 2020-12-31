/**
 * @file registers.h
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

#ifndef _LIBUNWIND_REGISTERS_H
#define _LIBUNWIND_REGISTERS_H

#include <stdint.h>

#define MAX_NUM_REGISTERS 32

class register_state
{
public:
    register_state() = default;
    virtual ~register_state() = default;
    register_state(register_state &&) noexcept = default;
    register_state(const register_state &) = default;
    register_state &operator=(register_state &&) noexcept = default;
    register_state &operator=(const register_state &) = default;

    virtual uint64_t get_ip() const
    {
        return 0;
    }

    virtual register_state &set_ip(uint64_t value)
    {
        (void)value;
        return *this;
    }

    virtual uint64_t get(uint64_t index) const
    {
        (void)index;
        return 0;
    }

    virtual register_state &set(uint64_t index, uint64_t value)
    {
        (void)index;
        (void)value;
        return *this;
    }

    virtual void commit()
    {
    }

    virtual void commit(uint64_t cfa)
    {
        (void)cfa;
    }

    virtual void resume()
    {
    }

    virtual uint64_t max_num_registers() const
    {
        return 0;
    }

    virtual const char *name(uint64_t index) const
    {
        (void)index;
        return "forgot to overload name";
    }

    virtual void dump() const
    {
    }

private:
};

#endif

//=============================================================================
// End of file
//=============================================================================