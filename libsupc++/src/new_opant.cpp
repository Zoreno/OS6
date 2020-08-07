#include <new>

#if __cpp_aligned_new

void *operator new(std::size_t size, std::align_val_t al, const std::nothrow_t &) noexcept
{
    try
    {
        return operator new(size, al);
    }
    catch (...)
    {
        return nullptr;
    }
}

#endif