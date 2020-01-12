#==============================================================================
# Unit testing cmake build file coverage module
#
# Configures and adds target for generating coverage reports fron the tests
#==============================================================================

set(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -coverage -fprofile-arcs -ftest-coverage")
set(GCC_COVERAGE_LINK_FLAGS    "-coverage -lgcov")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

add_custom_target(coverage
    COMMAND rm -f coverage.*
    #COMMAND lcov -c -i -d ../../../../libk -o "coverage.base"
    COMMAND lcov -c -d . -o "coverage.total"
    #COMMAND lcov -a "coverage.base" -a "coverage.captured" -o "coverage.total"
    COMMAND lcov -r "coverage.total" "*/gtest/*" "*/gmock/*" "*/OS6/unittest/*" "*/c++/*" -o "coverage.info"
    COMMAND genhtml "coverage.info" 
    COMMAND firefox index.html &
    DEPENDS tests-run)

#==============================================================================
# End of file
#==============================================================================