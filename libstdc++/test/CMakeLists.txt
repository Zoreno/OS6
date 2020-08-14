#==============================================================================
# Unit testing cmake build file
# This is built as a separate build as it uses the native compiler instead of
# the cross-compiler
#==============================================================================

cmake_minimum_required(VERSION 3.16)

project(libstdc++test)

include(cmake/compiler.cmake)
include(cmake/utest.cmake)

declare_test(integral_constant type_traits/test_integral_constant.cpp)

#==============================================================================
# End of file
#==============================================================================
