#==============================================================================
# Cmake file for compiling the OS6 operating system tests
#==============================================================================

project(tests)

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set(TEST_CFLAGS "-g -O0 -Wall -W -std=c11")
set(TEST_LDFLAGS "-fprofile-arcs")

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
	target_link_libraries(test-${NAME} PUBLIC m)
	add_test(NAME ${NAME} COMMAND test-${NAME})
endmacro(declare_test)

#==============================================================================
# Tests
#==============================================================================

# TODO: Move tests to separate folders to reduce name clash

declare_test(libk/ctype isalnum)

declare_test(libk/string memset)

declare_test(libk/stdlib strtod)

#==============================================================================
# End of file
#==============================================================================