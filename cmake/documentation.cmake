#==============================================================================
# Documentation
#==============================================================================

add_custom_target(docs
    COMMAND doxygen
    COMMAND gvfs-open docs/html/index.html 2> /dev/null &
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(cleandocs
    COMMAND rm -r docs/*
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
