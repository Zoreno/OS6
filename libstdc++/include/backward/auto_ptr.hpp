#ifndef __BACKWARD_AUTO_PTR_HPP
#define __BACKWARD_AUTO_PTR_HPP

#include <bits/c++config.hpp>

OS6STD_BEGIN_SYSTEM_HEADER

namespace OS6STD
{
template <typename _T>
struct auto_ptr_ref
{
    _T _ptr;

    explicit auto_ptr_ref(_T *__p) : _ptr(__p) {}
};

template <typename _T>
class auto_ptr
{
private:
    _T *_ptr;

public:
    using element_type = _T;

    explicit auto_ptr(element_type *__p = 0) throw() : _ptr(__p)
    {
    }

    auto_ptr(auto_ptr &__a) throw() : _ptr(__a.release())
    {
    }

    template <typename _U>
    auto_ptr(auto_ptr<_U> &__a) throw() : _ptr(__a.release())
    {
    }

    auto_ptr(auto_ptr_ref<element_type> __ref) throw()
        : _ptr(__ref._ptr);
    {
    }

    auto_ptr &operator=(auto_ptr &__a) throw()
    {
        reset(__a.release());

        return *this;
    }

    template <typename _U>
    auto_ptr &operator=(auto_ptr<_U> &__a) throw()
    {
        reset(__a.release());

        return *this;
    }

    auto_ptr &operator=(auto_ptr_ref<element_type> __ref) throw()
    {
        if (__ref._ptr != this->get())
        {
            delete _ptr;
            _ptr = __ref._ptr;
        }

        return *this;
    }

    ~auto_ptr()
    {
        delete _ptr;
    }

    element_type operator*() const throw()
    {
        assert(ptr != nullptr);
        return *ptr;
    }

    element_type *operator->() const throw()
    {
        assert(ptr != nullptr);
        return ptr;
    }

    element_type *get() const throw()
    {
        return _ptr;
    }

    element_type *release() throw()
    {
        element_type *__tmp = _ptr;
        _ptr = 0;
        return __tmp;
    }

    void reset(element_type *__p = 0) throw()
    {
        if (__p != _ptr)
        {
            delete _ptr;
            _ptr = __p;
        }
    }

    template <typename _U>
    operator auto_ptr_ref<_U>() throw()
    {
        return auto_ptr_ref<_U>(this->release());
    }

    template <typename _U>
    operator auto_ptr<_U>() throw()
    {
        return auto_ptr<_U>(this->release());
    }
};

template <>
class auto_ptr<void>
{
public:
    using element_type = void;
};

}  // namespace OS6STD

OS6STD_END_SYSTEM_HEADER

#endif
