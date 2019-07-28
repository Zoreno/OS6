#ifndef _CXXABI_H
#define _CXXABI_H

#pragma GCC system_header

#include <stdint.h>
#include <stddef.h>

namespace std
{
class type_info;
}

namespace __cxxabiv1
{
void __cxa_pure_virtual(void) __attribute__((__noreturn__));
void __cxa_deleted_virtual(void) __attribute__((__noreturn__));
} // namespace __cxxabiv1

extern "C"
{
#include <ia64_cxx_abi.h>

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
    void __cxa_throw(void *thrown_exception, std::type_info *tinfo, void (*dest)(void *));
    void *__cxa_begin_catch(_Unwind_Exception *exception);
    void __cxa_end_catch();

    _Unwind_Reason_Code __gxx_personality_v0(
        int version,
        _Unwind_Action actions,
        uint64_t exception_class,
        _Unwind_Exception *unwind_exception,
        _Unwind_Context *context);
}

#include <typeinfo>

namespace __cxxabiv1
{
class __fundamental_type_info : public std::type_info
{
public:
    explicit __fundamental_type_info(const char *__n) : std::type_info(__n) {}
    virtual ~__fundamental_type_info();
};

class __array_type_info : public std::type_info
{
public:
    explicit __array_type_info(const char *__n) : std::type_info(__n) {}
    virtual ~__array_type_info();
};

class __function_type_info : public std::type_info
{
public:
    explicit __function_type_info(const char *__n) : std::type_info(__n) {}

    virtual ~__function_type_info();

protected:
    virtual bool __is_function_p() const;
};

class __enum_type_info : public std::type_info
{
public:
    explicit __enum_type_info(const char *__n) : std::type_info(__n) {}
    virtual ~__enum_type_info();
};

class __pbase_type_info : public std::type_info
{
public:
    unsigned int __flags;
    const std::type_info *__pointee;

    explicit __pbase_type_info(const char *__n, int __quals,
                               const std::type_info *__type)
        : std::type_info(__n), __flags(__quals), __pointee(__type)
    {
    }

    virtual ~__pbase_type_info();

    enum __masks
    {
        __const_mask = 0x1,
        __volatile_mask = 0x2,
        __restrict_mask = 0x4,
        __incomplete_mask = 0x8,
        __incomplete_class_mask = 0x10,
        __transaction_safe_mask = 0x20,
        __noexcept_mask = 0x40
    };

protected:
    __pbase_type_info(const __pbase_type_info &);

    __pbase_type_info &operator=(const __pbase_type_info &);

    virtual bool __do_catch(const std::type_info *__thr_type, void **__thr_obj,
                            unsigned int __outer) const;

    inline virtual bool __pointer_catch(const __pbase_type_info *__thr_type, void **__thr_obj,
                                        unsigned __outer) const;
};

inline bool __pbase_type_info::__pointer_catch(const __pbase_type_info *thrown_type,
                                               void **thr_obj,
                                               unsigned outer) const
{
    return __pointee->__do_catch(thrown_type->__pointee, thr_obj, outer + 2);
}

class __pointer_type_info : public __pbase_type_info
{
public:
    explicit __pointer_type_info(const char *__n, int __quals,
                                 const std::type_info *__type)
        : __pbase_type_info(__n, __quals, __type) {}
    virtual ~__pointer_type_info();

protected:
    virtual bool __is_pointer_p() const;

    virtual bool __pointer_catch(const __pbase_type_info *__thr_type, void **__thr_obj,
                                 unsigned __outer) const;
};

class __class_type_info;

class __pointer_to_member_type_info : public __pbase_type_info
{
public:
    __class_type_info *__context;

    explicit __pointer_to_member_type_info(const char *__n, int __quals,
                                           const std::type_info *__type,
                                           __class_type_info *__class)
        : __pbase_type_info(__n, __quals, __type), __context(__class) {}

    virtual ~__pointer_to_member_type_info();

protected:
    __pointer_to_member_type_info(const __pointer_to_member_type_info &);

    __pointer_to_member_type_info &operator=(const __pointer_to_member_type_info &);

    virtual bool __pointer_catch(const __pbase_type_info *__thr_type, void **__thr_obj,
                                 unsigned __outer) const;
};

class __base_class_type_info
{
public:
    const __class_type_info *__base_type;

    long long __offset_flags;

    enum __offset_flags_masks
    {
        __virtual_mask = 0x1,
        __public_mask = 0x2,
        __hwm_bit = 2,
        __offset_shift = 8
    };

    bool __is_virtual_p() const
    {
        return __offset_flags & __virtual_mask;
    }

    bool __is_public_p() const
    {
        return __offset_flags & __public_mask;
    }

    ptrdiff_t __offset() const
    {
        return static_cast<ptrdiff_t>(__offset_flags) >> __offset_shift;
    }
};

class __class_type_info : public std::type_info
{
public:
    explicit __class_type_info(const char *__n) : type_info(__n) {}

    virtual ~__class_type_info();

    enum __sub_kind
    {
        __unknown = 0,
        __not_contained,
        __contained_ambig,
        __contained_virtual_mask = __base_class_type_info::__virtual_mask,
        __contained_public_mask = __base_class_type_info::__public_mask,
        __contained_mask = 1 << __base_class_type_info::__hwm_bit,
        __contained_private = __contained_mask,
        __contained_public = __contained_mask | __contained_public_mask
    };

    struct __upcast_result;
    struct __dyncast_result;

protected:
    virtual bool __do_upcast(const __class_type_info *__dst_type, void **__obj_ptr) const;

    virtual bool __do_catch(const type_info *__thr_type, void **__thr_obj, unsigned __outer) const;

public:
    virtual bool __do_upcast(const __class_type_info *__dst, const void *__obj,
                             __upcast_result &__restrict __result) const;

    inline __sub_kind __find_public_src(ptrdiff_t __src2dst, const void *__obj_ptr,
                                        const __class_type_info *__src_type,
                                        const void *__src_ptr) const;

    virtual bool __do_dyncast(ptrdiff_t __src2dst, __sub_kind __access_path,
                              const __class_type_info *__dst_type, const void *__obj_ptr,
                              const __class_type_info *__src_type, const void *__src_ptr,
                              __dyncast_result &__result) const;

    virtual __sub_kind __do_find_public_src(ptrdiff_t __src2dst, const void *__obj_ptr,
                                            const __class_type_info *__src_type,
                                            const void *__src_ptr) const;
};

class __si_class_type_info : public __class_type_info
{
public:
    const __class_type_info *__base_type;

    explicit __si_class_type_info(const char *__n, const __class_type_info *__base)
        : __class_type_info(__n), __base_type(__base) {}

    virtual ~__si_class_type_info();

protected:
    __si_class_type_info(const __si_class_type_info &);

    __si_class_type_info &operator=(const __si_class_type_info &);

    virtual bool __do_dyncast(ptrdiff_t __src2dst, __sub_kind __access_path,
                              const __class_type_info *__dst_type, const void *__obj_ptr,
                              const __class_type_info *__src_type, const void *__src_ptr,
                              __dyncast_result &__result) const;

    virtual __sub_kind __do_find_public_src(ptrdiff_t __src2dst, const void *__obj_ptr,
                                            const __class_type_info *__src_type,
                                            const void *__sub_ptr) const;

    virtual bool __do_upcast(const __class_type_info *__dst, const void *__obj,
                             __upcast_result &__restrict __result) const;
};

class __vmi_class_type_info : public __class_type_info
{
public:
    unsigned int __flags;
    unsigned int __base_count;

    __base_class_type_info __base_info[1];

    explicit __vmi_class_type_info(const char *__n, int ___flags)
        : __class_type_info(__n), __flags(___flags), __base_count(0) {}

    virtual ~__vmi_class_type_info();

    enum __flags_masks
    {
        __non_diamond_repeat_mask = 0x1,
        __diamond_shaped_mask = 0x2,
        __flags_unknown_mask = 0x10
    };

protected:
    virtual bool __do_dyncast(ptrdiff_t __src2dst, __sub_kind __access_path,
                              const __class_type_info *__dst_type, const void *__obj_ptr,
                              const __class_type_info *__src_type, const void *__src_ptr,
                              __dyncast_result &__result) const;

    virtual __sub_kind __do_find_public_src(ptrdiff_t __src2dst, const void *__obj_ptr,
                                            const __class_type_info *__src_type,
                                            const void *__src_ptr) const;

    virtual bool __do_upcast(const __class_type_info *__dst, const void *__obj,
                             __upcast_result &__restrict __result) const;
};

} // namespace __cxxabiv1

namespace abi = __cxxabiv1;

#endif