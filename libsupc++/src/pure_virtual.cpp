#include <cxxabi.h>

extern "C" void _exit(int val);

namespace __cxxabiv1
{

extern "C" void __cxa_pure_virtual(void)
{
    _exit(-1);

    __builtin_unreachable();
}

} // namespace __cxxabiv1