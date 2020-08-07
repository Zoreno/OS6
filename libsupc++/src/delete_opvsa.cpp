#include <new>

#if __cpp_sized_deallocation && __cpp_aligned_new

void operator delete[](void *ptr, std::size_t, std::align_val_t al) noexcept
{
    ::operator delete[](ptr, al);
}

#endif