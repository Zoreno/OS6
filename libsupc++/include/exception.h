#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#pragma GCC system_header

extern "C++"
{

    namespace std
    {
    class exception
    {
    public:
        exception() noexcept {}

        virtual ~exception() noexcept;

        exception(const exception &) = default;
        exception &operator=(const exception &) = default;
        exception(exception &&) = default;
        exception &operator=(exception &&) = default;

        virtual const char *what() const noexcept;
    };
    } // namespace std
}

#endif