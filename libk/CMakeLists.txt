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
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/strdup.c)

# ctype.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isalnum.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isalpha.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/iscntrl.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isgraph.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isdigit.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/islower.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isprint.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/ispunct.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isspace.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isupper.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isxdigit.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/tolower.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/toupper.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/isascii.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/ctype/toascii.c)

# stdlib.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/abs.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/bsearch.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdlib/div.c)
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
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/vsscanf.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/stdio/sscanf.c)

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

#==============================================================================
# Tests
#==============================================================================