#include <new>

std::bad_alloc::~bad_alloc()
{
}

const char *std::bad_alloc::what() const
{
    return "std::bad_alloc";
}