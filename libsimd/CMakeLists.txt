#==============================================================================
# Cmake file for compiling the OS6 operating system libsimd module
#==============================================================================

project(libsimd)

add_library(${LIBSIMD} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set_target_properties(${LIBSIMD} PROPERTIES COMPILE_FLAGS ${CFLAGS})
set_target_properties(${LIBSIMD} PROPERTIES SKIP_BUILD_RPATH 1)

#==============================================================================
# Path macros
#==============================================================================

#==============================================================================
# Includes and dependencies
#==============================================================================

target_include_directories(${LIBSIMD} SYSTEM PRIVATE ${LIBSIMD_INCLUDE})

# TODO: Make this not depend on libk for porting to userspace
target_include_directories(${LIBSIMD} SYSTEM PRIVATE ${LIBK_INCLUDE})

#==============================================================================
# Sources
#==============================================================================

target_sources(${LIBSIMD} PRIVATE ${LIBSIMD_SRC}/mm_alloc.c)

#==============================================================================
# Tests
#==============================================================================