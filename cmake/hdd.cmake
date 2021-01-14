#==============================================================================
# Build HDD
#==============================================================================

set(HDD_NAME disk.img)
set(HDD_IMAGE ${CMAKE_BINARY_DIR}/${HDD_NAME})

add_custom_command(OUTPUT ${HDD_NAME}
    DEPENDS ${ALL_APPS_LIST_DEST}
    COMMAND ../create_hdd.sh ${HDD_NAME} 100M
    COMMENT "Creating HDD"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})