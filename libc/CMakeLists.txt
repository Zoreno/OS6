#==============================================================================
# Cmake file for compiling the OS6 operating system libk module
#==============================================================================

project(libc)

add_library(${LIBC} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set_target_properties(${LIBC} PROPERTIES COMPILE_FLAGS ${CFLAGS})
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

# stdio.h header
target_sources(${LIBC} PRIVATE ${LIBC_SRC}/stdio/puts.c)

#==============================================================================
# Tests
#==============================================================================