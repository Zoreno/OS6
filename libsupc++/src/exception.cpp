#include <exception>

#include <new>

std::exception::~exception() noexcept {}

const char *std::exception::what() const noexcept
{
    return "std::exception";
}

std::bad_exception::~bad_exception() noexcept {}

const char *std::bad_exception::what() const noexcept
{
    return "std::bad_exception";
}