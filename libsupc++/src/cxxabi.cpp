#include <cxxabi.h>

#include <stdio.h>

#define EXCEPTION_BUFF_SIZE 255
char exception_buff[EXCEPTION_BUFF_SIZE];

extern "C"
{
    void *__cxa_allocate_exception(size_t thrown_size)
    {
        puts("Allocating exception\n");

        if (thrown_size > EXCEPTION_BUFF_SIZE)
        {
            puts("Exception too big\n");
        }

        return &exception_buff;
    }

    void __cxa_free_exception(void *thrown_exception)
    {
        puts("Freeing exception\n");
    }

    extern void _exit(int ret);

    void __cxa_throw(void *thrown_exception, struct type_info *tinfo, void (*dest)(void *))
    {
        puts("Throw!\n");

        __cxa_exception *header = ((__cxa_exception *)thrown_exception - 1);
        _Unwind_RaiseException(&header->unwind_header);

        puts("No one handled exception...\n");

        _exit(-145);
    }

    void __cxa_begin_catch()
    {
        puts("Begin catch");
    }

    void __cxa_end_catch()
    {
        puts("End catch");
    }

    _Unwind_Reason_Code __gxx_personality_v0(
        int version,
        _Unwind_Action actions,
        uint64_t exception_class,
        _Unwind_Exception *unwind_exception,
        _Unwind_Context *context)
    {
        puts("Personality");
    }
}