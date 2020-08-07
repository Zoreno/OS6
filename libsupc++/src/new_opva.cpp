#include <new>

#if __cpp_aligned_new

void *
operator new[](std::size_t sz, std::align_val_t al)
{
    return ::operator new(sz, al);
}

#endif