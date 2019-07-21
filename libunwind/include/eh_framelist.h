#ifndef _LIBUNWIND_EH_FRAMELIST_H
#define _LIBUNWIND_EH_FRAMELIST_H

#include <stdint.h>

struct eh_frame_t
{
    void *addr;
    uint64_t size;
};

extern "C" struct eh_frame_t *get_eh_frame_list() noexcept;

#endif