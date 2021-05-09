#==============================================================================
# Unit testing cmake build file gtest module
#
# Contains a macro for declaring a test
#==============================================================================

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

macro(declare_test name source)
add_executable(${name} ${source})
target_link_libraries(${name} ${GTEST_LIBRARIES} pthread)
add_test(${name} ./${name})
endmacro()

#==============================================================================
# End of file
#==============================================================================
