#include <new>

#if __cpp_aligned_new

void *operator new[](std::size_t sz, std::align_val_t al, const std::nothrow_t &) noexcept
{
    try
    {
        return ::operator new[](sz, al);
    }
    catch (...)
    {
        return nullptr;
    }
}

#endif