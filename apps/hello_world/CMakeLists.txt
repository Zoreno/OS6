#==============================================================================
# Cmake file for compiling the OS6 operating system apps
#==============================================================================

project(hello_world)

set(APP_NAME hello_world)

add_executable(${APP_NAME} "")

include("../user_apps_c.cmake")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

#==============================================================================
# Path macros
#==============================================================================

#==============================================================================
# Includes and dependencies
#==============================================================================

#==============================================================================
# Sources
#==============================================================================

target_sources(${APP_NAME} PRIVATE hello_world.c)

#==============================================================================
# End of file
#==============================================================================
