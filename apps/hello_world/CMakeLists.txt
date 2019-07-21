#==============================================================================
# Cmake file for compiling the OS6 operating system apps
#==============================================================================

project(hello_world)

set(APP_NAME hello_world)

add_executable(${APP_NAME} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/libc/linker.ld")

set_target_properties(${APP_NAME} PROPERTIES COMPILE_FLAGS ${CFLAGS})

set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS "-n -T ${LINKER_SCRIPT}")
set_target_properties(${APP_NAME} PROPERTIES DEPENDS ${LINKER_SCRIPT})

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

target_include_directories(${APP_NAME} SYSTEM PRIVATE ${LIBC_INCLUDE})

target_link_libraries(${APP_NAME} PRIVATE ${LIBC})

#==============================================================================
# Sources
#==============================================================================

target_sources(${APP_NAME} PRIVATE hello_world.c)

#==============================================================================
# End of file
#==============================================================================