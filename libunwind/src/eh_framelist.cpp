#include <eh_framelist.h>

#include <stdio.h>

extern unsigned long long int __start_eh_frame;
extern unsigned long long int __end_eh_frame;

static struct eh_frame_t frame;

extern "C" struct eh_frame_t *get_eh_frame_list() noexcept
{
    frame = {.addr = &__start_eh_frame, .size = (uint64_t)(&__end_eh_frame - &__start_eh_frame)};

    printf("__start_eh_frame = %#016x\n", &__start_eh_frame);
    printf("__end_eh_frame = %#016x\n", &__end_eh_frame);

    printf("frame.addr = %#016x\n", frame.addr);
    printf("frame.size = %#016x\n", frame.size);

    return &frame;
}