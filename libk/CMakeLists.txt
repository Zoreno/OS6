#==============================================================================
# Cmake file for compiling the OS6 operating system libk module
#==============================================================================

project(libk)

add_library(${LIBK} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set_target_properties(${LIBK} PROPERTIES COMPILE_FLAGS ${CFLAGS})
set_target_properties(${LIBK} PROPERTIES SKIP_BUILD_RPATH 1)

set(CMAKE_C_LINK_EXECUTABLE "ld <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} ${ASMFLAGS} -o <OBJECT> <SOURCE>")

#==============================================================================
# Path macros
#==============================================================================

#==============================================================================
# Includes and dependencies
#==============================================================================

target_include_directories(${LIBK} SYSTEM PRIVATE ${LIBK_INCLUDE})

#==============================================================================
# Sources
#==============================================================================

# string.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/bcmp.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/bcopy.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/bzero.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/ffs.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/ffsl.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/ffsll.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/fls.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/flsl.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/flsll.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/index.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/rindex.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memset.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memchr.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memcmp.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memcpy.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memcpyw.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memcpyl.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memmove.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memsetw.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memsetl.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memsetll.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memccpy.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memmem.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/mempcpy.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memrchr.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memfrob.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strcat.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strncat.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strchr.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strcmp.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strncmp.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strcpy.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strncpy.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strcspn.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strlen.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strnlen.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strpbrk.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strtok_r.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strtok.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strspn.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strstr.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strdup.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strrchr.c)

# ctype.h/wctype.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/__categories.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/__jp_to_uc.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isalnum_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isalnum.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isalpha_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isalpha.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iscntrl_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iscntrl.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isgraph_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isgraph.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isdigit_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isdigit.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/islower_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/islower.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isprint_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isprint.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/ispunct_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/ispunct.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isspace_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isspace.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isupper_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isupper.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isxdigit_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isxdigit.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isascii_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isascii.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/toascii_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/toascii.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/tolower_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/tolower.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/toupper_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/toupper.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswalnum_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswalnum.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswalpha_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswalpha.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswcntrl_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswcntrl.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswgraph_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswgraph.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswdigit_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswdigit.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswlower_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswlower.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswprint_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswprint.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswpunct_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswpunct.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswspace_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswspace.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswupper_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswupper.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswxdigit_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iswxdigit.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/wctype_l.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/wctype.c)

# stdlib.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__divdi3.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__divl.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__moddi3.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__nlz.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__sdiv64.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__smod64.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__udiv64.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__udivdi3.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__umod64.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/__umoddi3.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/abs.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/labs.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/llabs.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/bsearch.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/div.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/ldiv.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/lldiv.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/random.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/itoa.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/ltoa.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/lltoa.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/uitoa.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/ultoa.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/ulltoa.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/strtol.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/strtoul.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/strtoll.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/strtoull.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/atoi.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/atol.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/atoll.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/qsort.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/malloc.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/free.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/calloc.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/realloc.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/atof.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/strtod.c)

# stdio.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/fputc.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/fputs.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/fprintf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/set_stdout.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/printf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/putchar.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/puts.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/sprintf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/vsprintf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/getc.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/getchar.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/fgetc.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/fgets.c)

# libgen.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/libgen/basename.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/libgen/dirname.c)

# errno.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/errno/errno.c)

# wchar.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcwidth.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcsncpy.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcscmp.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcscat.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcsncat.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcslen.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcscpy.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcsspn.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcspbrk.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcschr.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcsrchr.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/wchar/wcstok.c)

# math.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/fmaxf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/fminf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/ceilf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/floorf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/fmodf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/fabsf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/fdimf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/nanf.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/rcpf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/sqrtf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/rsqrtf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/log2f.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/logf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/log10f.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/logbf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/pow2f.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/expf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/powf.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/sinf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/cosf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/tanf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/atanf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/asinf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/acosf.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/sinhf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/coshf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/tanhf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/atanhf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/asinhf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/acoshf.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/cscf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/secf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/cotf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/acscf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/asecf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/acotf.c)

target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/cschf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/sechf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/cothf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/acschf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/asechf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/math/acothf.c)

# fenv.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/fenv/fegetround.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/fenv/fesetround.c)

# setjmp.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/setjmp/setjmp.asm)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/setjmp/longjmp.asm)

# sys/syscall.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/sys/syscall/_gettimeofday.c)

# time.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/time/asctime.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/time/difftime.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/time/mktime.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/time/time.c)

#==============================================================================
# Tests
#==============================================================================
