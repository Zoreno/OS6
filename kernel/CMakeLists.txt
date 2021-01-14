#==============================================================================
# Cmake file for compiling the OS6 operating system kernel module
#==============================================================================

project(kernel)

add_executable(kernel "")

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

include(kernel)

#==============================================================================
# Includes and dependencies
#==============================================================================

target_include_directories(kernel SYSTEM PRIVATE ${KERNEL_INCLUDE})
target_include_directories(kernel SYSTEM PRIVATE ${LIBK_INCLUDE})
target_include_directories(kernel SYSTEM PRIVATE ${LIBSIMD_INCLUDE})

target_link_libraries(kernel PRIVATE ${LIBK})
target_link_libraries(kernel PRIVATE ${LIBSIMD})

#==============================================================================
# Sources
#==============================================================================

kernel_subdirectory(src)

#==============================================================================
# End of file
#==============================================================================