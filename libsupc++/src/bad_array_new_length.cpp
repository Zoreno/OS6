#include <new>

std::bad_array_new_length::~bad_array_new_length() {}

const char *std::bad_array_new_length::what() const
{
    return "std::bad_array_new_length";
}