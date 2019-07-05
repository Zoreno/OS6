#==============================================================================
# Cmake file for compiling the OS6 operating system tests
#==============================================================================

project(tests)

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set(TEST_CFLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
set(TEST_LDFLAGS "-fprofile-arcs -ftest-coverage")

#==============================================================================
# Path macros
#==============================================================================

#==============================================================================
# Test macro
#==============================================================================

macro(declare_test PATH NAME)
	add_executable(test-${NAME} ${OS6_TESTS}/${PATH}/test-${NAME}.c ${OS6_TESTS}/driver/jtest.c)
	target_include_directories(test-${NAME} SYSTEM PRIVATE ${OS6_TESTS}/driver)
	set_target_properties(test-${NAME} PROPERTIES COMPILE_FLAGS ${TEST_CFLAGS})
	set_target_properties(test-${NAME} PROPERTIES LINK_FLAGS ${TEST_LDFLAGS})
	add_test(NAME ${NAME} COMMAND test-${NAME})
endmacro(declare_test)

#==============================================================================
# Tests
#==============================================================================

# TODO: Move tests to separate folders to reduce name clash

declare_test(libk/ctype isalnum)

declare_test(libk/string memset)

#==============================================================================
# End of file
#==============================================================================