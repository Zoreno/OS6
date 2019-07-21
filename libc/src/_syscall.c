#include <_syscall.h>

int64_t do_syscall0(int64_t syscall)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall)
                     : "memory");

    return ret;
}

int64_t do_syscall1(int64_t syscall, int64_t arg1)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall),
                       "b"(arg1)
                     : "memory");

    return ret;
}

int64_t do_syscall2(int64_t syscall, int64_t arg1, int64_t arg2)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall),
                       "b"(arg1),
                       "c"(arg2)
                     : "memory");

    return ret;
}

int64_t do_syscall3(int64_t syscall, int64_t arg1, int64_t arg2, int64_t arg3)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall),
                       "b"(arg1),
                       "c"(arg2),
                       "d"(arg3)
                     : "memory");

    return ret;
}