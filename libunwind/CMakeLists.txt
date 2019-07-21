#==============================================================================
# Cmake file for compiling the OS6 operating system stack unwinding module
#==============================================================================

project(libunwind)

add_library(${LIBUNWIND} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set_target_properties(${LIBUNWIND} PROPERTIES COMPILE_FLAGS ${CPPFLAGS})
set_target_properties(${LIBUNWIND} PROPERTIES SKIP_BUILD_RPATH 1)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} ${ASMFLAGS} -o <OBJECT> <SOURCE>")

#==============================================================================
# Path macros
#==============================================================================

#==============================================================================
# Includes and dependencies
#==============================================================================

target_include_directories(${LIBUNWIND} SYSTEM PRIVATE ${LIBUNWIND_INCLUDE})
target_include_directories(${LIBUNWIND} SYSTEM PRIVATE ${LIBC_INCLUDE})

target_link_libraries(${LIBUNWIND} PRIVATE ${LIBC})

#==============================================================================
# Sources
#==============================================================================

target_sources(${LIBUNWIND} PRIVATE ${LIBUNWIND_SRC}/eh_frame.cpp)
target_sources(${LIBUNWIND} PRIVATE ${LIBUNWIND_SRC}/dwarf4.cpp)
target_sources(${LIBUNWIND} PRIVATE ${LIBUNWIND_SRC}/registers_intel_x64.asm)
target_sources(${LIBUNWIND} PRIVATE ${LIBUNWIND_SRC}/ia64_cxx_abi.cpp)

#==============================================================================
# Tests
#==============================================================================