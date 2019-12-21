#==============================================================================
# Cmake file for compiling the OS6 operating system
#==============================================================================

cmake_minimum_required(VERSION 3.12)

set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "asm")

set(TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/toolchain")

if(NOT EXISTS "${TOOLCHAIN_DIR}/OS6_cross_cc")
message(FATAL_ERROR "Did not find cross-compiler directory. Did you run ./build_toolchain.sh from the ${TOOLCHAIN_DIR} directory?")
endif()

set(OS6_COMPILER_PATH "${TOOLCHAIN_DIR}/OS6_cross_cc/x86_64-elf/bin")

set(CMAKE_C_COMPILER "${OS6_COMPILER_PATH}/gcc")
set(CMAKE_CXX_COMPILER "${OS6_COMPILER_PATH}/g++")

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_CROSSCOMPILING 1)

project(OS6 C CXX ASM_NASM)

set(CAN_USE_ASSEMBLER TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

enable_language(C)
enable_language(CXX)
enable_language(ASM_NASM)

#enable_testing()

#==============================================================================
# Flags and compiler configuration
#==============================================================================

# Debug build
set(DEBUG_BUILD true)

set(CFLAGS "-m64")

# Set warnings and error conditions
set(CFLAGS "${CFLAGS} -Wall -Wextra -Werror=implicit-function-declaration -Wno-unused -Wno-sign-compare")

# Disable standard library
set(CFLAGS "${CFLAGS} -ffreestanding")

# Disable language features
set(CFLAGS "${CFLAGS} -fno-builtin -finline-functions -fno-stack-protector")

set(CPPFLAGS "${CFLAGS} -std=c++11 -static-libgcc")

# Set C standard
set(CFLAGS "${CFLAGS} -Wno-pointer-sign -std=c11")

# Code coverage
set(CFLAGS "${CFLAGS} -g")

set(ASMFLAGS "")

if(${DEBUG_BUILD})

    # Setup dwarf information
    set(CFLAGS "${CFLAGS} -DOS6_DEBUG -gdwarf-4")
    set(ASMFLAGS "${ASMFLAGS} -g -F dwarf")

else()

    # Set full optimization 
    set(CFLAGS "${CFLAGS} -DOS6_RELEASE -O3")
    set(ASMFLAGS "${ASMFLAGS} -Ox")

endif()

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


set(OS6_TESTS ${CMAKE_SOURCE_DIR}/tests)

set(KERNEL_EXEC kernel.bin)
set(LIBK k)
set(LIBC c)
set(LIBSUPCXX supc++)
set(LIBUNWIND unwind)
set(LIBSIMD simd)

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

#==============================================================================
# Apps
#==============================================================================

define_property(GLOBAL PROPERTY APPS_LIST
    BRIEF_DOCS "Global list of applications"
    FULL_DOCS "Global list of applications")
set_property(GLOBAL PROPERTY APPS_LIST "")

macro(listappend var)
    set_property(GLOBAL APPEND PROPERTY APPS_LIST "${var}")
endmacro(listappend)

add_subdirectory(apps)

get_property(ALL_APPS_LIST GLOBAL PROPERTY APPS_LIST)

#message(${ALL_APPS_LIST})

# Copy the list and prepend the output directory
set(ALL_APPS_LIST_DEST ${ALL_APPS_LIST})
list(TRANSFORM ALL_APPS_LIST_DEST PREPEND "${CMAKE_SOURCE_DIR}/root/bin/")

#message(${ALL_APPS_LIST_DEST})

#==============================================================================
# Tests
#==============================================================================

add_subdirectory(tests)

#==============================================================================
# Build ISO
#==============================================================================

set(ISO_NAME os.iso)
set(ISO_IMAGE ${ISO_NAME})

foreach(app ${ALL_APPS_LIST})
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/root/bin/${app}
        DEPENDS apps/${app}/${app}
        COMMAND cp -u apps/${app}/${app} ${CMAKE_SOURCE_DIR}/root/bin/${app}
        COMMENT "Copying executable ${app}")
endforeach()

add_custom_command(OUTPUT ${ISO_IMAGE}
    DEPENDS kernel/kernel.bin
    COMMAND cp -u kernel/kernel.bin ${CMAKE_SOURCE_DIR}/mnt/boot
    COMMAND grub-mkrescue -o ${ISO_IMAGE} ${CMAKE_SOURCE_DIR}/mnt
    COMMENT "Creating ISO image"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#==============================================================================
# Build HDD
#==============================================================================

set(HDD_NAME disk.img)
set(HDD_IMAGE ${CMAKE_BINARY_DIR}/${HDD_NAME})

add_custom_command(OUTPUT ${HDD_NAME}
    DEPENDS ${ALL_APPS_LIST_DEST}
    COMMAND ../create_hdd.sh ${HDD_NAME} 100M
    COMMENT "Creating HDD"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#==============================================================================
# Custom targets for running QEMU
#==============================================================================

# Clear flags
set(QEMU_FLAGS)

#set(QEMU_FLAGS ${QEMU_FLAGS} --trace events=/tmp/events)

set(QEMU_FLAGS ${QEMU_FLAGS} -cpu Nehalem)

# Set memory flags
set(QEMU_FLAGS ${QEMU_FLAGS} -m 512M)

# Set RTC flags
set(QEMU_FLAGS ${QEMU_FLAGS} -rtc base=localtime)

# Set boot deivce
set(QEMU_FLAGS ${QEMU_FLAGS} -boot d)

# Set USB devices
#set(QEMU_FLAGS ${QEMU_FLAGS} -usb -device usb-ehci,id=ehci -device usb-tablet,bus=ehci.0,port=1 -device usb-kbd,bus=ehci.0,port=2)
#set(QEMU_FLAGS ${QEMU_FLAGS} -usb -device usb-mouse -device usb-kbd)

#set(QEMU_FLAGS ${QEMU_FLAGS} -usb -device usb-tablet)

# Set serial interface
set(QEMU_FLAGS ${QEMU_FLAGS} -serial stdio)

# Set display settings
set(QEMU_FLAGS ${QEMU_FLAGS} -vga std)

#Set storage devices
set(QEMU_FLAGS ${QEMU_FLAGS} -drive file=${HDD_NAME},format=raw -cdrom ${ISO_IMAGE})

add_custom_target(run
    COMMAND export SDL_VIDEO_X11_DGAMOUSE=0
    COMMAND qemu-system-x86_64 ${QEMU_FLAGS}
    DEPENDS ${ISO_IMAGE} ${HDD_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(debug
    COMMAND export SDL_VIDEO_X11_DGAMOUSE=0
    COMMAND qemu-system-x86_64 -s -S ${QEMU_FLAGS} &
    COMMAND sudo ddd --gdb -command=${CMAKE_SOURCE_DIR}/gdbdebug 2> /dev/null
    DEPENDS ${ISO_IMAGE} ${HDD_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#==============================================================================
# Documentation
#==============================================================================

add_custom_target(docs
    COMMAND doxygen
    COMMAND gvfs-open docs/html/index.html 2> /dev/null &
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(cleandocs
    COMMAND rm -r docs/*
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

#==============================================================================
# Custom testing command
#==============================================================================

add_custom_target(run-tests
    COMMAND make
    COMMAND ctest -VV --output-on-failure
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	
#==============================================================================
# End of file
#==============================================================================