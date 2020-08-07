#==============================================================================
# Cmake file for compiling the OS6 operating system libk module
#==============================================================================

project(libc)

add_library(${LIBC} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set_target_properties(${LIBC} PROPERTIES COMPILE_FLAGS "${CFLAGS} -fpie")
set_target_properties(${LIBC} PROPERTIES LINKER_LANGUAGE C)
set_target_properties(${LIBC} PROPERTIES SKIP_BUILD_RPATH 1)

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

target_include_directories(${LIBC} SYSTEM PRIVATE ${LIBC_INCLUDE})

#==============================================================================
# Sources
#==============================================================================

target_sources(${LIBC} PRIVATE ${LIBC_SRC}/crt0.asm)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/crti.asm)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/crtn.asm)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/_libc_init.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/_syscall.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/fini.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/init.c)

# ctype.h header

target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/isalnum.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/isalpha.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/isascii.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/iscntrl.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/isdigit.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/isgraph.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/islower.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/isprint.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/ispunct.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/isspace.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/isupper.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/isxdigit.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/toascii.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/tolower.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/ctype/toupper.c)

# stdio.h header
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdio/puts.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdio/printf.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdio/vsprintf.c)

# stdlib.h header
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/abs.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/bsearch.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/div.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/random.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/itoa.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/ltoa.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/lltoa.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/uitoa.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/ultoa.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/ulltoa.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/strtol.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/strtoul.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/strtoll.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/strtoull.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/atoi.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/atol.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/atoll.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/qsort.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/malloc.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/free.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/calloc.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/realloc.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/atof.c)
#target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdlib/strtod.c)


# string.h header
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memset.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memchr.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memcmp.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memcpy.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memcpyw.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memcpyl.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memmove.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memsetw.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memsetl.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memsetll.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memccpy.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memmem.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/mempcpy.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memrchr.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/memfrob.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strcat.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strncat.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strchr.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strcmp.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strncmp.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strcpy.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strncpy.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strcspn.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strlen.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strnlen.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strpbrk.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strtok_r.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strspn.c)
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/string/strdup.c)


#==============================================================================
# Tests
#==============================================================================