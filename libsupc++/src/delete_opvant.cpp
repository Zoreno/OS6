#include <new>

#if __cpp_aligned_new

void operator delete[](void *ptr, std::align_val_t al, const std::nothrow_t &) noexcept
{
    ::operator delete[](ptr, al);
}

#endif