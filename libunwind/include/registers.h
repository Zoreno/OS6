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