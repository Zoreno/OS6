
#==============================================================================
# Apps
#==============================================================================

define_property(GLOBAL PROPERTY APPS_LIST
    BRIEF_DOCS "Global list of applications"
    FULL_DOCS "Global list of applications")
set_property(GLOBAL PROPERTY APPS_LIST "")

macro(listappend var)
    set_property(GLOBAL APPEND PROPERTY APPS_LIST "${var}")
endmacro(listappend)

add_subdirectory(apps)

get_property(ALL_APPS_LIST GLOBAL PROPERTY APPS_LIST)

# Copy the list and prepend the output directory
set(ALL_APPS_LIST_DEST ${ALL_APPS_LIST})
list(TRANSFORM ALL_APPS_LIST_DEST PREPEND "${CMAKE_SOURCE_DIR}/root/bin/")
