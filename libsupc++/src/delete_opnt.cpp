#include <new>

void operator delete(void *ptr, const std::nothrow_t &)noexcept
{
    ::operator delete(ptr);
}