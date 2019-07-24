#include <new>

namespace std
{
extern void *malloc(size_t size);
}

void *operator new(std::size_t size)
{
    return std::malloc(size);
}