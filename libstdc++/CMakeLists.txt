#==============================================================================
# Cmake file for compiling the OS6 operating system c++ support library module
#==============================================================================

project(libstdc++)

add_library(${LIBSTDCXX} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set_target_properties(${LIBSTDCXX} PROPERTIES COMPILE_FLAGS ${CPPFLAGS})
set_target_properties(${LIBSTDCXX} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${LIBSTDCXX} PROPERTIES SKIP_BUILD_RPATH 1)

set(CMAKE_CXX_LINK_EXECUTABLE "${OS6_COMPILER_PATH}/ld <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} ${ASMFLAGS} -o <OBJECT> <SOURCE>")

#==============================================================================
# Path macros
#==============================================================================

#==============================================================================
# Includes and dependencies
#==============================================================================

target_include_directories(${LIBSTDCXX} SYSTEM PRIVATE ${LIBC_INCLUDE})
target_include_directories(${LIBSTDCXX} SYSTEM PRIVATE ${LIBSTDCXX_INCLUDE})
target_include_directories(${LIBSTDCXX} SYSTEM PRIVATE ${LIBSTDCXX_INCLUDE}/std)
target_include_directories(${LIBSTDCXX} SYSTEM PRIVATE ${LIBSTDCXX_INCLUDE}/c)
target_include_directories(${LIBSTDCXX} SYSTEM PRIVATE ${LIBSUPCXX_INCLUDE})

target_link_libraries(${LIBSTDCXX} PRIVATE ${LIBC})
target_link_libraries(${LIBSTDCXX} PRIVATE ${LIBSUPCXX})

#==============================================================================
# Sources
#==============================================================================

target_sources(${LIBSTDCXX} PRIVATE ${LIBSTDCXX_SRC}/dummy.cpp)

#==============================================================================
# Tests
#==============================================================================

set(LIBSTDCXX_TESTS ${LIBSTDCXX_SRC}/../tests)

target_sources(${LIBSTDCXX} PRIVATE ${LIBSTDCXX_TESTS}/type_traits/integral_constant/integral_constant_test.cpp)
target_sources(${LIBSTDCXX} PRIVATE ${LIBSTDCXX_TESTS}/runner.cpp)
