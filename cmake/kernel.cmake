#==============================================================================
# Flags and compiler configuration
#==============================================================================

set_target_properties(kernel PROPERTIES COMPILE_FLAGS ${CFLAGS})
set_target_properties(kernel PROPERTIES LINKER_LANGUAGE C)
set_target_properties(kernel PROPERTIES LINK_FLAGS "-n -T ${LINKER_SCRIPT}")
set_target_properties(kernel PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

set(CMAKE_C_LINK_EXECUTABLE "${OS6_LINKER} <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> \
    <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} ${ASMFLAGS} -o <OBJECT> <SOURCE>")

#==============================================================================
# Macros for specifying source files.
#==============================================================================

macro(kernel_source file)
    target_sources(kernel PRIVATE ${CMAKE_CURRENT_LIST_DIR}/${file})
endmacro()

macro(kernel_subdirectory dir)
    include(${CMAKE_CURRENT_LIST_DIR}/${dir}/sources.cmake)
endmacro()

#==============================================================================
# End file
#==============================================================================
