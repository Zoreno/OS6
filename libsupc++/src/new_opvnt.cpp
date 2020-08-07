#include <new>

void *operator new[](std::size_t sz, const std::nothrow_t &) noexcept
{
    try
    {
        return ::operator new[](sz);
    }
    catch (...)
    {
        return nullptr;
    }
}