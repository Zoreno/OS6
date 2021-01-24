#==============================================================================
# Build ISO
#==============================================================================

set(ISO_NAME os.iso)
set(ISO_IMAGE ${ISO_NAME})

foreach(app ${ALL_APPS_LIST})
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/root/bin/${app}
        DEPENDS apps/${app}/${app}
        COMMAND cp -u apps/${app}/${app} ${CMAKE_SOURCE_DIR}/root/bin/${app}
        COMMENT "Copying executable ${app}")
endforeach()

add_custom_command(OUTPUT ${ISO_IMAGE}
    DEPENDS kernel/kernel
    COMMAND cp -u kernel/kernel ${CMAKE_SOURCE_DIR}/mnt/boot/kernel.bin
    COMMAND grub-mkrescue -o ${ISO_IMAGE} ${CMAKE_SOURCE_DIR}/mnt
    COMMENT "Creating ISO image"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})