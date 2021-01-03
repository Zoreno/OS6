#==============================================================================
# Cmake file for utest based testing
#==============================================================================

enable_testing()

#
# Contains all boilerplate for setting up a test executable with given
# name and source file(s).
#
# @param name Name of the test
# @param source List of all files with test cases
#
macro(declare_test name source)
add_executable(${name} tests/${source})
target_include_directories(${name} BEFORE PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(${name} BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/../include")
target_include_directories(${name} BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/../include/std")
add_test(${name} ./${name})
endmacro()

#
# Contains all boilerplate for setting up a test executable with given
# name and source file(s) including a main function.
#
# @param name Name of the test
# @param source List of all files with test cases
#
macro(declare_test_main name source)
add_executable(${name} tests/${source} utest_main.c)
target_include_directories(${name} BEFORE PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(${name} BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/../include")
target_include_directories(${name} BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/../include/std")
add_test(${name} ./${name})
endmacro()

#==============================================================================
# End of file
#==============================================================================
