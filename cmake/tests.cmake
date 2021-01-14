#==============================================================================
# Tests
#==============================================================================

if(${BUILDING_TESTS})

include(ExternalProject)

ExternalProject_Add(unittest
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/unittest
    INSTALL_COMMAND ""
    BUILD_ALWAYS ON)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "unittest")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "unittest-prefix")

add_custom_target(tests-run 
    DEPENDS unittest
    COMMAND make tests-run
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unittest-prefix/src/unittest-build)

add_custom_target(coverage 
    DEPENDS unittest
    COMMAND make coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unittest-prefix/src/unittest-build)

endif()
