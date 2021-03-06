#==============================================================================
# Custom targets for running QEMU
#==============================================================================

# Clear flags
set(QEMU_FLAGS)

#set(QEMU_FLAGS ${QEMU_FLAGS} --trace events=/tmp/events)

set(QEMU_FLAGS ${QEMU_FLAGS} -d guest_errors -D qemu.log)

set(QEMU_FLAGS ${QEMU_FLAGS} -cpu Nehalem)
#set(QEMU_FLAGS ${QEMU_FLAGS} -enable-kvm -cpu host)

# Set memory flags
set(QEMU_FLAGS ${QEMU_FLAGS} -m 512M)

# Set RTC flags
set(QEMU_FLAGS ${QEMU_FLAGS} -rtc base=localtime)

# Set boot deivce
set(QEMU_FLAGS ${QEMU_FLAGS} -boot d)

# Set USB devices
set(QEMU_FLAGS ${QEMU_FLAGS} -usb -device usb-ehci,id=ehci)
set(QEMU_FLAGS ${QEMU_FLAGS} -device usb-tablet,bus=ehci.0,port=1)
set(QEMU_FLAGS ${QEMU_FLAGS} -device usb-kbd,bus=ehci.0,port=2)

# Set serial interface
set(QEMU_FLAGS ${QEMU_FLAGS} -serial stdio)

# Set display settings
set(QEMU_FLAGS ${QEMU_FLAGS} -vga std)

# Set ACPI flags
#set(QEMU_FLAGS ${QEMU_FLAGS} -no-acpi)

# Set bios
# set(QEMU_FLAGS ${QEMU_FLAGS} -bios ${CMAKE_SOURCE_DIR}/../seabios/out/bios.bin)

#Set storage devices
set(QEMU_FLAGS ${QEMU_FLAGS} -drive file=${HDD_NAME},format=raw -cdrom ${ISO_IMAGE})

add_custom_target(run
    COMMAND export SDL_VIDEO_X11_DGAMOUSE=0
    COMMAND qemu-system-x86_64 ${QEMU_FLAGS}
    DEPENDS ${ISO_IMAGE} ${HDD_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(debug
    COMMAND export SDL_VIDEO_X11_DGAMOUSE=0
    COMMAND qemu-system-x86_64 -s -S ${QEMU_FLAGS} &
    COMMAND sudo ddd --gdb -command=${CMAKE_SOURCE_DIR}/gdbdebug 2> /dev/null
    DEPENDS ${ISO_IMAGE} ${HDD_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})