#==============================================================================
# Unit testing cmake build file
# This is built as a separate build as it uses the native compiler instead of
# the cross-compiler
#==============================================================================

cmake_minimum_required(VERSION 3.16)

project(libstdc++test)

include(cmake/compiler.cmake)
include(cmake/utest.cmake)

declare_test(type_traits_integral_constant type_traits/test_integral_constant.cpp)
declare_test(type_traits_is_void type_traits/test_is_void.cpp)

#==============================================================================
# End of file
#==============================================================================
