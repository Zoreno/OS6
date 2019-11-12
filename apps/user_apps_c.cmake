#==============================================================================
# @file: user_apps_c.cmake
# 
# @brief: Cmake module for setting compiler and path settings for C
# applications.
#==============================================================================


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
# Custom targets and commands
#==============================================================================

add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND sha512sum -b ${APP_NAME} > ${APP_NAME}.hash
    COMMENT "Generating checksum for ${APP_NAME}")

set_property(GLOBAL APPEND PROPERTY APPS_LIST "${APP_NAME}")

#==============================================================================
# End of file
#==============================================================================