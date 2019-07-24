#include <new>

namespace std
{
extern "C" void free(void *);
} // namespace std

void operator delete(void *ptr) noexcept
{
    return std::free(ptr);
}