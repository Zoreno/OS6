#==============================================================================
# Custom targets for running QEMU
#==============================================================================

# Clear flags
set(QEMU_FLAGS)

#set(QEMU_FLAGS ${QEMU_FLAGS} --trace events=/tmp/events)

set(QEMU_FLAGS ${QEMU_FLAGS} -d guest_errors -D qemu.log)

set(QEMU_FLAGS ${QEMU_FLAGS} -cpu Nehalem -smp 2)
#set(QEMU_FLAGS ${QEMU_FLAGS} -enable-kvm -cpu host)

# Set memory flags
set(QEMU_FLAGS ${QEMU_FLAGS} -m 512M)

# Set RTC flags
set(QEMU_FLAGS ${QEMU_FLAGS} -rtc base=localtime)

# Set boot deivce
set(QEMU_FLAGS ${QEMU_FLAGS} -boot d)

set(QEMU_FLAGS ${QEMU_FLAGS} -net user)
#set(QEMU_FLAGS ${QEMU_FLAGS} -netdev hubport,id=u1,hubid=0)
set(QEMU_FLAGS ${QEMU_FLAGS} -netdev tap,id=u1)
#set(QEMU_FLAGS ${QEMU_FLAGS} -netdev user,id=u1,net=192.168.1.0/24,dhcpstart=192.168.1.9)
set(QEMU_FLAGS ${QEMU_FLAGS} -device e1000,netdev=u1)
set(QEMU_FLAGS ${QEMU_FLAGS} -object filter-dump,id=f1,netdev=u1,file=qemu-e1000e.pcap)

set(QEMU_FLAGS ${QEMU_FLAGS} -enable-kvm)

# Set USB devices
set(QEMU_FLAGS ${QEMU_FLAGS} -usb -device usb-ehci,id=ehci)
set(QEMU_FLAGS ${QEMU_FLAGS} -device usb-tablet,bus=ehci.0,port=1)
set(QEMU_FLAGS ${QEMU_FLAGS} -device usb-kbd,bus=ehci.0,port=2)

# Set serial interface
set(QEMU_FLAGS ${QEMU_FLAGS} -serial stdio)

# Set display settings
set(QEMU_FLAGS ${QEMU_FLAGS} -vga std)

# Set audio settings
#set(QEMU_FLAGS ${QEMU_FLAGS} -device ich9-intel-hda,addr=1f.1 -audiodev pa,id=snd0 -device hda-output,audiodev=snd0)

# RNG device (not used)
#set(QEMU_FLAGS ${QEMU_FLAGS} -object rng-random,id=rng0,filename=/dev/urandom -device virtio-rng-pci,rng=rng0)

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
