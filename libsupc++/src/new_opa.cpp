#include <new>

#if __cpp_aligned_new

void *operator new(std::size_t size, std::align_val_t)
{
    // TODO: Implement
    return (void *)0;
}

#endif