#include <new>

void operator delete[](void *ptr) noexcept
{
    ::operator delete(ptr);
}