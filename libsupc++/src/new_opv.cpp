#include <new>

void *operator new[](std::size_t size)
{
    return ::operator new(size);
}