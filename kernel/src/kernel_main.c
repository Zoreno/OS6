/**
 * @file kernel_main.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 * 
 * @brief Kernel C entry point
 * 
 * @copyright Copyright (C) 2019,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 * 
 */

#include <debug/debug_terminal.h>

#include <boot/multiboot2.h>

#include <arch/arch.h>

#include <mm/phys_mem.h>
#include <mm/memory_info.h>
#include <mm/virt_mem.h>
#include <mm/kheap.h>

#include <serial/serial.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <util/list.h>
#include <util/tree.h>

#include <drivers/ide.h>
#include <drivers/blockdev.h>
#include <drivers/vbe.h>
#include <drivers/keyboard_ps2.h>
#include <drivers/mouse_ps2.h>

#include <vfs/vfs.h>
#include <vfs/ext2.h>

#include <cmos/cmos_rtc.h>

#include <util/hexdump.h>

#include <exec/elf64.h>

#include <debug/backtrace.h>

#include <pci/pci.h>

#include <ctype.h>

#include <gui/gui.h>
#include <gui/terminal_mode.h>

#include <assert.h>

#include <immintrin.h>

#include <syscall/syscall.h>

#include <process/process.h>

#include <util/json.h>

#include <simple_cli/simple_cli.h>

/*

Global todo list:

Kernel
 - Kernel Events
   - Mouse moved
   - Keyboard pressed
   - Time tick
 - Proper shutdown handling

Libk
 - time.h
 - Fork / POSIX Threading

ACPI
 - Shutdown

Multitasking:
 - Threads (Clone)
 - Processes (Fork)
 - Mutexes/Semaphores
 - Shared memory
 - Float/SIMD state save/restore
 - Address space separation
 - Sleep
 - Working directory

Load Executables:
 - ELF files
 - Libraries
    - System calls
    - libc
    - libstdc++
    - c++ support lib
    - unistd
    - libos6 (Window management, keyboard input, network)
    - libutil
    - getopt
    - 2D rendering library (SFML-like)
    - 3D rendering library (Softrender/OpenGL-like)
 - Command line parameters
 - Working directory

USB Drivers
 - Get the drivers to work
 - Mouse
 - Keyboard
 - Mass storage device

Networking
 - E1000 Network card
 - Ethernet
 - IPv4
 - UDP
 - TCP
 - Sockets
 - HTTP
 - FTP

ATAPI
 - Driver

ISO9660
 - FS on OS image
 . May be used to install the OS on HDD

VFS
 - Create random device
 - Create procfs
 - Fix proper listing of mounted paths
 - Pipes
 - Serial device
 - RAMdevice (May not be needed as the VFS is initialized early. Still nice for completion)

Sound
 - Sound card driver
 - Kernel interface

Kernel terminal
 - Launch applications

GUI
 - Double buffering (SIMD-blit)
 - Widgets
   - Buttons
   - Alt-bars
   - Textbox
   - Label
   - Checkbox
   - Radio button
   - Scroll bar
   - Panels and tabs
   - Canvas (rendering area)
   - Progress bar
   - Slider
   - Dropdown
 - Application ownership (add window to process resource table, unique window handle, inter-process communication)
 - Input queue
 - Scroll
 - Change theme
 - Change time format
 - Color struct and conversion functions
 - Image loading
 - Change background
 - Allow application to get rendering bitmap
 - Add debug limit checking in context
 - Create message box framework
 - Window Icon
 - Sizing problem
 - Separate horizontal and vertical resize
 - Context menu
 - Window settings as separate struct
 - Window Events that the application can subscribe to

Applications
 - terminal
 - cat
 - cp
 - echo
 - date
 - true
 - false
 - head
 - kill
 - killall
 - ls
 - mkdir
 - mv
 - rm
 - readelf
 - reboot
 - shutdown
 - pwd
 - sleep
 - sort
 - stat
 - touch
 - uname
 - ln
 - chmod
 - chown
 - head
 - tail

 - jedit
 - jinterp
 - jweb
 - calculator

*/

extern void run_unit_tests();

// https://www.gnu.org/software/grub/manual/multiboot2/html_node/kernel_002ec.html

extern void *__kernel_start;
extern void *__kernel_end;

// TODO: Move to separate file
void parse_multiboot(unsigned char *mb_ptr, memory_info_t *mem_info)
{

    struct multiboot_start_tag *start_tag = (struct multiboot_start_tag *)mb_ptr;

    uint32_t mb_size = start_tag->total_size;

    printf("Multiboot total size: %i\n", mb_size);

    int end_found = 0;

    mb_ptr += sizeof(struct multiboot_start_tag);

    while (!end_found)
    {
        struct multiboot_tag *tag = (struct multiboot_tag *)mb_ptr;

        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_CMDLINE:
        {
            struct multiboot_tag_string *cmdline = (struct multiboot_tag_string *)tag;
            printf("[Command line] \"%s\"\n", cmdline->string);
        }
        break;

        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
        {
            struct multiboot_tag_string *boot_loader = (struct multiboot_tag_string *)tag;
            printf("[Boot loader name] \"%s\"\n", boot_loader->string);
        }
        break;
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
        {
            struct multiboot_tag_basic_meminfo *basic_mem =
                (struct multiboot_tag_basic_meminfo *)tag;
            printf("[Basic memory] lower: %i upper: %i\n", basic_mem->mem_lower, basic_mem->mem_upper);
        }
        break;
        case MULTIBOOT_TAG_TYPE_BOOTDEV:
        {
            struct multiboot_tag_bootdev *bootdev = (struct multiboot_tag_bootdev *)tag;

            printf("[Bootdev] biosdev: %i, partition: %i, sub_partition: %i\n",
                   bootdev->biosdev, bootdev->slice, bootdev->part);
        }
        break;
        case MULTIBOOT_TAG_TYPE_MMAP:
        {
            struct multiboot_tag_mmap *mmap = (struct multiboot_tag_mmap *)tag;

            int count = mmap->size / mmap->entry_size;

            printf("[mmap] entry_size: %i, count: %i\n", mmap->entry_size, count);

            int j = 0;

            for (int i = 0; i < count; ++i)
            {
                struct multiboot_mmap_entry mmap_entry = mmap->entries[i];

                printf("[mmap_entry] addr: %#016x len: %#016x type: %i\n",
                       mmap_entry.addr, mmap_entry.len, mmap_entry.type);

                if (mmap_entry.type == MULTIBOOT_MEMORY_AVAILABLE)
                {
                    mem_info->regions[j].addr = mmap_entry.addr;
                    mem_info->regions[j].len = mmap_entry.len;

                    ++j;
                }
            }
        }
        break;
        case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
        {
            struct multiboot_tag_load_base_addr *lba = (struct multiboot_tag_load_base_addr *)tag;

            printf("[Load Base Address] addr: %#08x\n", lba->load_base_addr);
        }
        break;
        case MULTIBOOT_TAG_TYPE_END:
        {
            end_found = 1;
        }
        break;
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
        {
            struct multiboot_tag_elf_sections *elf_sections = (struct multiboot_tag_elf_sections *)tag;

            init_kernel_symbol_context(elf_sections, mem_info);
        }
        break;
        default:
        {
            printf("[Tag] type: %i size: %i\n", tag->type, tag->size);
        }
        break;
        }

        if (tag->type == MULTIBOOT_TAG_TYPE_END)
        {
            end_found = 1;
        }

        mb_ptr += (tag->size + 7) & ~7;
    }

    uint64_t largest_mem = 0;

    for (int i = 0; i < MEM_INFO_MAX_REGIONS; ++i)
    {
        mem_region_t region = mem_info->regions[i];

        uint64_t region_end = region.addr + region.len;

        if (region_end > largest_mem)
        {
            largest_mem = region_end;
        }
    }

    mem_info->memory_size = largest_mem;

    mem_info->kernel_load_addr = (uint64_t)&__kernel_start;
    //mem_info->kernel_end = (uint64_t)&__kernel_end;
    mem_info->kernel_size = mem_info->kernel_end - mem_info->kernel_load_addr;
}


void mouse_moved_handler(mouse_moved_event_t *event)
{
    static int i = 0;

    printf("Mouse moved [%i]: %i, %i\n", i++, (int64_t)event->x, (int64_t)event->y);
}


spinlock_t print_lock;

int kernel_main(unsigned long long rbx, unsigned long long rax)
{
    (void)rax;

    serial_init_full();

    debug_terminal_initialize();

    set_stdout(scom1_fd);

    printf("Initializing kernel...\n");

    run_unit_tests();

    arch_initialize();

    RTC_init();

    memory_info_t mem_info;

    memset(&mem_info, 0, sizeof(mem_info));

    parse_multiboot((unsigned char *)rbx, &mem_info);

    printf("Kernel end: %#016x\n", mem_info.kernel_end);

    phys_mem_init(&mem_info);

    virt_mem_initialize();

    sti();

    keyboard_install();
    mouse_install();

    kheap_init();

    vfs_install();

    init_ide_devices();

    ext2_initialize();

    pciInit();

    vbe_bochs_set_gfx(800, 600, 4);

    syscall_install();

    tasking_install();

    //exec_elf("bin/hello_world", 0, NULL, NULL, 0);

    // printf("My pid: %d\n", pid);

    // fork();

    // switch_task(1);

    spinlock_init(&print_lock);

    //printf("After switch\n");

    pid_t pid = process_get_pid();

    pid_t fork_ret = process_fork();

    if (process_get_pid() != pid)
    {
        exec_elf("bin/hello_world_cpp", 0, NULL, NULL, 0);

        printf("[ERROR] Exec elf returned\n");
    }

    /*
    pid = process_get_pid();

    process_fork();

    if (process_get_pid() != pid)
    {
        exec_elf("bin/hello_world", 0, NULL, NULL, 0);

        printf("[ERROR] Exec elf returned\n");
    }

    //debug_print_process_tree();

    for (;;)
    {
        process_sleep(1000);

        pid_t pid = process_get_pid();

        spinlock_lock(&print_lock);
        printf("My pid: %d\n", pid);
        spinlock_unlock(&print_lock);
    }
    */

    printf("Kernel initailization done!\n");

    printf("================================================================================\n");
    printf("|| Launching GUI...                                                           ||\n");
    printf("================================================================================\n");

    //gui_init();

    //terminal_init();

    simple_cli_init();

#if 0

    int ret = mkdir_fs("newdir", 0);

    printf("mkdir_fs returned %i\n", ret);

    ret = create_file_fs("/newdir/testfile3", 0);

    printf("create_file_fs returned %i\n", ret);

    const char *pathw = "/newdir/testfile3";

    fs_node_t *nodew = kopen(pathw, 0);

    if (!nodew)
    {
        printf("Could not open %s", pathw);

        for (;;)
            ;
    }

    if (!(nodew->flags & FS_FILE))
    {
        printf("Not a file\n");

        for (;;)
            ;
    }

    const char *string = "This the new string that should be stored in the file";

    int bytes_written;

    // Discard current content of file
    //truncate_fs(nodew);

    // Write new content.
    bytes_written = write_fs(nodew, 0, strlen(string), string);

    if (bytes_written != strlen(string))
    {
        printf("Error writing to file\n");
    }

#endif

#if 0

    const char *path = "/dev/zero";

    fs_node_t *node = kopen(path, 0);

    if (!node)
    {
        printf("Could not open %s", path);

        for (;;)
            ;
    }

    if (!(node->flags & FS_FILE))
    {
        printf("Not a file\n");

        for (;;)
            ;
    }

    printf("%s opened!\n", path);
    printf("Size: %i\n", node->length);

    uint32_t size = 128;

    uint8_t *file = malloc(size);
    int bytes_read;

    bytes_read = read_fs(node, 0, size, file);

    if (bytes_read != size)
    {
        printf("Error reading from file\n");
    }

    hexdump(file, size);

#endif

#if 0

    const char *pathd = "/dev";

    fs_node_t *noded = kopen(pathd, 0);

    if (!noded)
    {
        printf("Could not open %s\n", pathd);

        for (;;)
            ;
    }

    printf("Contents of %s\n", pathd);

    if (!noded->flags & FS_DIRECTORY)
    {
        printf("Not a directory!\n");

        for (;;)
            ;
    }

    int index = 0;
    struct dirent *d;

    while ((d = readdir_fs(noded, index++)) != NULL)
    {
        printf("%s\n", d->name);
    }

#endif

    for (;;)
        __asm__ volatile("hlt");
}

//=============================================================================
// End of file
//=============================================================================