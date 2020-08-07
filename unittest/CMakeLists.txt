#==============================================================================
# Unit testing cmake build file
# This is built as a separate build as it uses the native compiler instead of
# the cross-compiler
#==============================================================================

cmake_minimum_required(VERSION 3.12)

project(OS6_TESTS C CXX ASM_NASM)

enable_testing()

#==============================================================================
# GTest
#==============================================================================

include(cmake/gtest.cmake)

#==============================================================================
# Coverage
#==============================================================================

include(cmake/coverage.cmake)

#==============================================================================
# All test declarations
#==============================================================================

declare_test(libk_strtod_test libk/stdlib/strtod_test.cpp)

declare_test(libk_memset_test libk/string/memset_test.cpp)
declare_test(libk_strlen_test libk/string/strlen_test.cpp)

#==============================================================================
# Run all tests target
#==============================================================================

add_custom_target(tests-run
    COMMAND ctest --output-on-failure -VV
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#==============================================================================
# End of file
#==============================================================================