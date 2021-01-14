#==============================================================================
# Cmake file for compiling the OS6 operating system
#==============================================================================

cmake_minimum_required(VERSION 3.12)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "asm")

include(toolchain)

project(OS6 C CXX ASM_NASM)

set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cflags)

#==============================================================================
# Path macros
#==============================================================================

set(KERNEL_INCLUDE ${CMAKE_SOURCE_DIR}/kernel/include)
set(KERNEL_SRC ${CMAKE_SOURCE_DIR}/kernel/src)

set(LIBK_INCLUDE ${CMAKE_SOURCE_DIR}/libk/include)
set(LIBK_SRC ${CMAKE_SOURCE_DIR}/libk/src)

set(LIBC_INCLUDE ${CMAKE_SOURCE_DIR}/libc/include)
set(LIBC_SRC ${CMAKE_SOURCE_DIR}/libc/src)

set(LIBSIMD_INCLUDE ${CMAKE_SOURCE_DIR}/libsimd/include)
set(LIBSIMD_SRC ${CMAKE_SOURCE_DIR}/libsimd/src)

set(LIBSUPCXX_INCLUDE ${CMAKE_SOURCE_DIR}/libsupc++/include)
set(LIBSUPCXX_SRC ${CMAKE_SOURCE_DIR}/libsupc++/src)

set(LIBUNWIND_INCLUDE ${CMAKE_SOURCE_DIR}/libunwind/include)
set(LIBUNWIND_SRC ${CMAKE_SOURCE_DIR}/libunwind/src)

set(LIBSTDCXX_INCLUDE ${CMAKE_SOURCE_DIR}/libstdc++/include)
set(LIBSTDCXX_SRC ${CMAKE_SOURCE_DIR}/libstdc++/src)

set(OS6_TESTS ${CMAKE_SOURCE_DIR}/tests)

set(KERNEL_EXEC kernel.bin)
set(LIBK k)
set(LIBC c)
set(LIBSUPCXX supc++)
set(LIBUNWIND unwind)
set(LIBSIMD simd)
set(LIBSTDCXX stdc++)

#==============================================================================
# LIBK kernel C standard library target
#==============================================================================

add_subdirectory(libk)

#==============================================================================
# libsimd target
#==============================================================================

add_subdirectory(libsimd)

#==============================================================================
# Kernel target
#==============================================================================

add_subdirectory(kernel)

#==============================================================================
# libc
#==============================================================================

add_subdirectory(libc)

#==============================================================================
# c++ support
#==============================================================================

add_subdirectory(libunwind)
add_subdirectory(libsupc++)
add_subdirectory(libstdc++)

include(tests)
include(applications)
include(iso)
include(hdd)
include(qemu)
include(documentation)

#==============================================================================
# End of file
#==============================================================================