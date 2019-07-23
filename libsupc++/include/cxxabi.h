#ifndef _CXXABI_H
#define _CXXABI_H

#pragma GCC system_header

#include <stdint.h>

extern "C"
{
#include <ia64_cxx_abi.h>
#include <typeinfo>

    typedef void (*unexpected_handler_t)(void);
    typedef void (*terminate_handler_t)(void);

    struct __cxa_exception
    {
        std::type_info *exception_type;
        void (*exception_destructor)(void *);
        unexpected_handler_t unexpected_handler;
        terminate_handler_t terminate_handler;
        __cxa_exception *next_exception;

        int handler_count;
        int handler_switch_value;
        const char *action_record;
        const char *language_specific_data;
        void *catch_temp;
        void *adjusted_ptr;

        _Unwind_Exception unwind_header;
    };

    void *__cxa_allocate_exception(size_t thrown_size);
    void __cxa_free_exception(void *thrown_exception);
    void __cxa_throw(void *thrown_exception, struct type_info *tinfo, void (*dest)(void *));
    void __cxa_begin_catch();
    void __cxa_end_catch();

    _Unwind_Reason_Code __gxx_personality_v0(
        int version,
        _Unwind_Action actions,
        uint64_t exception_class,
        _Unwind_Exception *unwind_exception,
        _Unwind_Context *context);
}

namespace abi = __cxxabiv1;

#endif