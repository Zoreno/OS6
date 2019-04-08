#==============================================================================
# Cmake file for compiling the OS6 operating system
#==============================================================================

cmake_minimum_required(VERSION 3.5)

set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "asm")
set(CMAKE_C_COMPILER gcc)

project(OS6 C ASM_NASM)

set(CAN_USE_ASSEMBLER TRUE)


enable_language(ASM_NASM)

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set(CFLAGS "-ffreestanding -std=c11 -Wall -Wextra -ffreestanding -fno-builtin \
-nostdlib -nostdinc -finline-functions -fno-stack-protector -fomit-frame-pointer \
-Werror=implicit-function-declaration -mno-sse -fno-exceptions -g -nodefaultlibs")
set(CMAKE_C_FLAGS "${CFLAGS}")
set(CMAKE_C_LINK_EXECUTABLE "ld <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/kernel/linker.ld")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -g -F dwarf -o <OBJECT> <SOURCE>")

#==============================================================================
# Path macros
#==============================================================================

set(KERNEL_INCLUDE ${CMAKE_SOURCE_DIR}/kernel/include)
set(KERNEL_SRC ${CMAKE_SOURCE_DIR}/kernel/src)

set(LIBK_INCLUDE ${CMAKE_SOURCE_DIR}/libk/include)
set(LIBK_SRC ${CMAKE_SOURCE_DIR}/libk/src)

set(KERNEL_EXEC kernel.bin)
set(LIBK k)

#==============================================================================
# LIBK kernel C standard library target
#==============================================================================

add_library(${LIBK} "")

# string.h header
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memset.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memchr.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memcmp.c)
target_sources(${LIBK} PRIVATE ${LIBK_SRC}/string/memcpy.c)
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


target_include_directories(${LIBK} SYSTEM PRIVATE ${LIBK_INCLUDE})

set_target_properties(${LIBK} PROPERTIES SKIP_BUILD_RPATH 1)

#==============================================================================
# Kernel target
#==============================================================================

add_executable(${KERNEL_EXEC} "")

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/boot/boot.asm)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/debug/debug_terminal.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/test/unit_tests.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/arch.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/cpu.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/gdt.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/idt.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/gdt_flush.asm)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/idt_flush.asm)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/default_irq_handler.asm)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/pic.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/pit.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/atomic.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/mm/phys_mem.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/mm/mm_bitmap.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/mm/virt_mem.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/mm/kheap.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/sync/spinlock.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/serial/serial.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/vfs/vfs.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/vfs/ext2.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/cmos/cmos_rtc.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/drivers/blockdev.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/drivers/ide.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/list.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/tree.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/kernel_main.c)

target_include_directories(${KERNEL_EXEC} SYSTEM PRIVATE ${KERNEL_INCLUDE})
target_include_directories(${KERNEL_EXEC} SYSTEM PRIVATE ${LIBK_INCLUDE})

target_link_libraries(${KERNEL_EXEC} PRIVATE ${LIBK})

set_target_properties(${KERNEL_EXEC} PROPERTIES LINKER_LANGUAGE C)
set_target_properties(${KERNEL_EXEC} PROPERTIES LINK_FLAGS "-n -T ${LINKER_SCRIPT}")
set_target_properties(${KERNEL_EXEC} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

#==============================================================================
# Build ISO
#==============================================================================


set(ISO_NAME os.iso)
set(ISO_IMAGE ${CMAKE_BINARY_DIR}/${ISO_NAME})

add_custom_command(OUTPUT ${ISO_IMAGE}
    POST_BUILD
    DEPENDS kernel.bin
    COMMAND cp kernel.bin ${CMAKE_SOURCE_DIR}/mnt/boot
    COMMAND grub-mkrescue -o ${ISO_IMAGE} ${CMAKE_SOURCE_DIR}/mnt
    COMMENT "Creating ISO image"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#==============================================================================
# Build HDD
#==============================================================================

set(HDD_NAME disk.img)
set(HDD_IMAGE ${CMAKE_BINARY_DIR}/${HDD_NAME})

add_custom_target(${HDD_NAME}
    COMMAND ../create_hdd.sh ${HDD_NAME} 100M
    COMMENT "Creating HDD"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#==============================================================================
# Custom targets for running QEMU
#==============================================================================

add_custom_target(run
    COMMAND qemu-system-x86_64 -m 512M -rtc base=localtime -boot d -serial stdio -drive file=${HDD_NAME},format=raw -cdrom ${ISO_IMAGE}
    DEPENDS ${ISO_IMAGE} ${HDD_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(debug
    COMMAND qemu-system-x86_64 -s -S ${ISO_IMAGE} &
    COMMAND sudo ddd --gdb -command=${CMAKE_SOURCE_DIR}/gdbdebug 2> /dev/null
    DEPENDS ${ISO_IMAGE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#==============================================================================
# End of file
#==============================================================================
