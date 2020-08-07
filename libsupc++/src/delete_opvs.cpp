#include <new>

#if __cpp_sized_deallocation

void operator delete[](void *ptr, std::size_t) noexcept
{
    ::operator delete[](ptr);
}

#endif