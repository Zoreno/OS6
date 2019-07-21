#ifndef _LIBUNWIND_MISC_H
#define _LIBUNWIND_MISC_H

#include <stdint.h>
#include <abort.h>

inline uint64_t bfabs(int64_t value)
{
    return value >= 0 ? static_cast<uint64_t>(value) : static_cast<uint64_t>(-value);
}

inline uint64_t add_offset(uint64_t value, int64_t offset)
{
    auto abs_offset = bfabs(offset);

    if (offset >= 0)
    {
        return value + abs_offset;
    }

    if (value >= abs_offset)
    {
        return value - abs_offset;
    }

    ABORT("attempted add an offset that would result in overflow");
    return 0;
}

#endif