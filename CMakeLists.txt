cmake_minimum_required(VERSION 3.5)

set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "asm")
set(CMAKE_C_COMPILER gcc)

project(OS6 C ASM_NASM)

set(CAN_USE_ASSEMBLER TRUE)


enable_language(ASM_NASM)


set(CFLAGS "-ffreestanding -std=c11 -Wall -Wextra -ffreestanding -fno-builtin \
-nostdlib -nostdinc -finline-functions -fno-stack-protector -fomit-frame-pointer \
-Werror=implicit-function-declaration -mno-sse -fno-exceptions -g")
set(CMAKE_C_FLAGS "${CFLAGS}")
set(CMAKE_C_LINK_EXECUTABLE "ld <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/kernel/linker.ld")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

add_executable(kernel.bin kernel/boot.asm kernel/kernel_main.c)

set_target_properties(kernel.bin PROPERTIES LINKER_LANGUAGE C)
set_target_properties(kernel.bin PROPERTIES LINK_FLAGS "-n -T ${LINKER_SCRIPT}")
set_target_properties(kernel.bin PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

set(ISO_NAME os.iso)
set(ISO_IMAGE ${CMAKE_BINARY_DIR}/${ISO_NAME})

add_custom_command(OUTPUT ${ISO_IMAGE}
    POST_BUILD
    DEPENDS kernel.bin
    COMMAND cp kernel.bin ${CMAKE_SOURCE_DIR}/mnt/boot
    COMMAND grub-mkrescue -o ${ISO_IMAGE} ${CMAKE_SOURCE_DIR}/mnt
    COMMENT "Creating ISO image"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(run
    COMMAND qemu-system-x86_64 ${ISO_IMAGE}
    DEPENDS ${ISO_IMAGE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})