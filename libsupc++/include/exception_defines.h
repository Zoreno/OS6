#ifndef _EXCEPTION_DEFINES_H
#define _EXCEPTION_DEFINES_H

#pragma GCC system_header

#if !__cpp_exceptions
#define __try if (true)
#define __catch if (false)
#define __throw_exception_again
#else
#define __try try
#define __catch catch
#define __throw_exception_again throw
#endif

#endif