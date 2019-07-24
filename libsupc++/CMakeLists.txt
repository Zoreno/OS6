#==============================================================================
# Cmake file for compiling the OS6 operating system c++ support library module
#==============================================================================

project(libsupc++)

add_library(${LIBSUPCXX} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set_target_properties(${LIBSUPCXX} PROPERTIES COMPILE_FLAGS ${CPPFLAGS})
set_target_properties(${LIBSUPCXX} PROPERTIES SKIP_BUILD_RPATH 1)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} ${ASMFLAGS} -o <OBJECT> <SOURCE>")

#==============================================================================
# Path macros
#==============================================================================

#==============================================================================
# Includes and dependencies
#==============================================================================

target_include_directories(${LIBSUPCXX} SYSTEM PRIVATE ${LIBSUPCXX_INCLUDE})
target_include_directories(${LIBSUPCXX} SYSTEM PRIVATE ${LIBUNWIND_INCLUDE})
target_include_directories(${LIBSUPCXX} SYSTEM PRIVATE ${LIBC_INCLUDE})

target_link_libraries(${LIBSUPCXX} PRIVATE ${LIBC})
target_link_libraries(${LIBSUPCXX} PRIVATE ${LIBUNWIND})

#==============================================================================
# Sources
#==============================================================================

target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/cxxabi.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/exception.cpp)

target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_op.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_op.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opv.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_opv.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/pure_virtual.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/deleted_virtual.cpp)

#==============================================================================
# Tests
#==============================================================================