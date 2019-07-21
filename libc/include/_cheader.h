#ifndef _LIBC_CHEADER_H
#define _LIBC_CHEADER_H

#ifdef __cplusplus
#define _c_header_begin \
    extern "C"          \
    {
#define _c_header_end }
#else
#define _c_header_begin
#define _c_header_end
#endif

#endif