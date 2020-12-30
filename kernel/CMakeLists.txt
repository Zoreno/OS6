#==============================================================================
# Cmake file for compiling the OS6 operating system kernel module
#==============================================================================

project(kernel)

add_executable(${KERNEL_EXEC} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/kernel/linker.ld")

set_target_properties(${KERNEL_EXEC} PROPERTIES COMPILE_FLAGS ${CFLAGS})
set_target_properties(${KERNEL_EXEC} PROPERTIES LINKER_LANGUAGE C)
set_target_properties(${KERNEL_EXEC} PROPERTIES LINK_FLAGS "-n -T ${LINKER_SCRIPT}")
set_target_properties(${KERNEL_EXEC} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

set(CMAKE_C_LINK_EXECUTABLE "ld <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} ${ASMFLAGS} -o <OBJECT> <SOURCE>")

#==============================================================================
# Path macros
#==============================================================================

#==============================================================================
# Includes and dependencies
#==============================================================================

target_include_directories(${KERNEL_EXEC} SYSTEM PRIVATE ${KERNEL_INCLUDE})
target_include_directories(${KERNEL_EXEC} SYSTEM PRIVATE ${LIBK_INCLUDE})
target_include_directories(${KERNEL_EXEC} SYSTEM PRIVATE ${LIBSIMD_INCLUDE})

target_link_libraries(${KERNEL_EXEC} PRIVATE ${LIBK})
target_link_libraries(${KERNEL_EXEC} PRIVATE ${LIBSIMD})

#==============================================================================
# Sources
#==============================================================================

# TODO: Sort all of these
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/acpi/acpi.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/boot/boot.asm)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/debug/debug_terminal.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/debug/backtrace.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/test/unit_tests.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/arch.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/cpu.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/gdt.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/idt.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/gdt_flush.asm)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/idt_flush.asm)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/default_irq_handler.asm)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/pic.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/pit.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/atomic.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/arch/x86-64/fpu.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/mm/phys_mem.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/mm/mm_bitmap.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/mm/virt_mem.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/mm/kheap.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/pci/pci_io.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/pci/pci_device.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/pci/pci.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/sync/spinlock.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/serial/serial.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/vfs/vfs.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/vfs/ext2.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/vfs/nulldev.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/vfs/zerodev.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/cmos/cmos_rtc.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/drivers/blockdev.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/drivers/ide.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/drivers/vbe.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/drivers/keyboard_ps2.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/drivers/mouse_ps2.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/exec/elf64.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/exec/exec_elf.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/process/process.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/process/read_ip.asm)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/process/switch_task.asm)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/process/launch_program.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/list.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/tree.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/hexdump.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/bitset.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/link.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/sha512.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/vector.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/hashmap.c)

#target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/json/json_number.c)
#target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/json/json_string.c)
#target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/util/json/json_utils.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/gui_listnode.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/gui_list.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/rect.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/context.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/window.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/desktop.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/gui.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/terminal_mode.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/terminal/terminal.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/terminal/terminal_buffer.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/terminal/terminal_context.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/gui/terminal/terminal_color.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/usb/usb_controller.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/usb/usb_descriptors.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/usb/usb_device.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/usb/usb_driver.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/usb/usb_hub.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/usb/usb.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/usb/usb_uhci.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/usb/usb_ehci.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscall.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/do_syscall.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/__stat_node.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_access.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_chdir.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_chmod.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_chown.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_close.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_debug_print.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_exit.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_getcwd.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_getpid.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_gettid.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_ioctl.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_lstat.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_mkdir.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_open.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_read.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_readdir.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_readlink.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_sbrk.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_seek.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_sleep.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_stat.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_statf.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_symlink.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_unlink.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_write.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/syscall/syscalls/syscall_yield.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/simple_cli/simple_cli.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/simple_cli/commands/cd_command.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/simple_cli/commands/ls_command.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/simple_cli/commands/mkdir_command.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/simple_cli/commands/pwd_command.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/simple_cli/commands/rm_command.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/simple_cli/commands/test_command.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/simple_cli/commands/exit_command.c)
target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/simple_cli/commands/time_command.c)

target_sources(${KERNEL_EXEC} PRIVATE ${KERNEL_SRC}/kernel_main.c)


#==============================================================================
# Tests
#==============================================================================