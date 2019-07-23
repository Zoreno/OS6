#include <eh_framelist.h>

extern unsigned long long int __start_eh_frame;
extern unsigned long long int __end_eh_frame;

static struct eh_frame_t frame = {.addr = &__start_eh_frame, .size = (uint64_t)(&__end_eh_frame - &__start_eh_frame)};

extern "C" struct eh_frame_t *get_eh_frame_list() noexcept
{

    return &frame;
}