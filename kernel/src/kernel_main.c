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

int64_t do_syscall0(int64_t syscall)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall)
                     : "memory");

    return ret;
}

int64_t do_syscall1(int64_t syscall, int64_t arg1)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall),
                       "b"(arg1)
                     : "memory");

    return ret;
}

int64_t do_syscall2(int64_t syscall, int64_t arg1, int64_t arg2)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall),
                       "b"(arg1),
                       "c"(arg2)
                     : "memory");

    return ret;
}

int64_t do_syscall3(int64_t syscall, int64_t arg1, int64_t arg2, int64_t arg3)
{
    int64_t ret;

    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(syscall),
                       "b"(arg1),
                       "c"(arg2),
                       "d"(arg3)
                     : "memory");

    return ret;
}

spinlock_t print_lock;

const char *simple_cli_get_user_name();
const char *simple_cli_get_user_group();
const char *simple_cli_get_working_directory();

char *simple_cli_read_line();
const char **simple_cli_split_line(char *line);
int simple_cli_get_token_count(const char **tokens);
int simple_cli_run_command(const char *name, int argc, const char **argv);
int simple_cli_is_command(const char *name);
int simple_cli_launch(char **args);
void simple_cli_init();

const char *simple_cli_get_user_name()
{
    return "root";
}

const char *simple_cli_get_user_group()
{
    return "root";
}

const char *simple_cli_get_working_directory()
{
    return process_get_current()->wd_path;
}

char *simple_cli_read_line()
{
    char c;

    const int buffer_size = 1024;

    char *buffer = malloc(buffer_size);
    int buf_ptr = 0;

    if (!buffer)
    {
        printf("[ERROR] Allocating buffer\n");
        for (;;)
            ;
    }

    memset(buffer, 0, buffer_size);

    char line_start[256] = {0};
    sprintf(line_start, "%s@%s:%s$ ", simple_cli_get_user_name(),
            simple_cli_get_user_group(),
            simple_cli_get_working_directory());

    printf("%s", line_start);

    while (1)
    {
        c = getc();

        if (c == 13)
        {
            putchar('\n');
            buffer[buf_ptr] = 0;
            return buffer;
        }
        else if (c == 127)
        {
            if (buf_ptr > 0)
            {
                buf_ptr--;
                putchar(13);
                printf("%s", line_start);
                buffer[buf_ptr] = 0;
                printf("%s ", buffer);
                putchar(13);
                printf("%s", line_start);
                printf("%s", buffer);
            }
        }
        else
        {
            putchar(c);
            buffer[buf_ptr++] = c;
        }
    }
}

const char **simple_cli_split_line(char *line)
{
    const char *strtok_delim = " \t\r\n\a";
    const int max_tokens_increment = 64;
    int max_tokens = max_tokens_increment;
    int position = 0;

    char **tokens = malloc(max_tokens * sizeof(char *));
    char *token;

    if (!tokens)
    {
        printf("[ERROR] Allocating buffer\n");
        for (;;)
            ;
    }

    token = strtok(line, strtok_delim);

    while (token != NULL)
    {
        tokens[position++] = token;

        if (position >= max_tokens)
        {
            max_tokens += max_tokens_increment;
            tokens = realloc(tokens, max_tokens * sizeof(char *));
            if (!tokens)
            {
                printf("[ERROR] Allocating buffer\n");
                for (;;)
                    ;
            }
        }

        token = strtok(NULL, strtok_delim);
    }

    tokens[position] = NULL;
    return (const char **)tokens;
}

int simple_cli_get_token_count(const char **tokens)
{
    int i = 0;
    while (tokens[i] != NULL)
    {
        ++i;
    }

    return i;
}

typedef int (*simple_cli_command_func_t)(int, const char **);

typedef struct _simple_cli_command
{
    const char *name;
    simple_cli_command_func_t command;
} simple_cli_command_t;

int test_command(int argc, const char **argv)
{
    printf("Test command\n");

    return 0;
}

int cd_command(int argc, const char **argv)
{
    if (argc != 2)
    {
        printf("Usage: cd <path>\n");
        return -1;
    }

    return syscall_chdir((char *)argv[1]);
}

static void ls_command_format_permissions(uint32_t permissions, char *buffer)
{
    sprintf(buffer, "%c%c%c%c%c%c%c%c%c",
            permissions & S_IRUSR ? 'r' : '-',
            permissions & S_IWUSR ? 'w' : '-',
            permissions & S_IXUSR ? 'x' : '-',

            permissions & S_IRGRP ? 'r' : '-',
            permissions & S_IWGRP ? 'w' : '-',
            permissions & S_IXGRP ? 'x' : '-',

            permissions & S_IROTH ? 'r' : '-',
            permissions & S_IWOTH ? 'w' : '-',
            permissions & S_IXOTH ? 'x' : '-');
}

static void ls_command_format_size(int human_readable, size_t size, char *buffer)
{
    if (human_readable)
    {
        if (size >= (1 << 20))
        {
            size_t t = size / (1 << 20);
            size_t td = (size - t * (1 << 20)) / ((1 << 20) / 10);
            sprintf(buffer, "%d.%1dM", (int)t, (int)td);
            return;
        }
        else if (size >= (1 << 10))
        {
            size_t t = size / (1 << 10);
            size_t td = (size - t * (1 << 10)) / ((1 << 10) / 10);
            sprintf(buffer, "%d.%1dK", (int)t, (int)td);
            return;
        }
    }

    sprintf(buffer, "%d", (int)size);
}

int ls_command(int argc, const char **argv)
{
    int fd = syscall_open(simple_cli_get_working_directory(), O_DIRECTORY, O_RDONLY);

    if (fd < 0)
    {
        return fd;
    }

    int i = 0;

    struct dirent dirent;

    while (syscall_readdir(fd, i, &dirent) == 1)
    {
        struct stat _stat;

        syscall_lstat(dirent.name, (uintptr_t)&_stat);

        char mode_char = '-';

        if (S_ISDIR(_stat.st_mode))
        {
            mode_char = 'd';
        }

        if (S_ISCHR(_stat.st_mode))
        {
            mode_char = 'c';
        }

        if (S_ISBLK(_stat.st_mode))
        {
            mode_char = 'b';
        }

        if (S_ISFIFO(_stat.st_mode))
        {
            mode_char = 'p';
        }

        char permissons_string[10] = {0};

        ls_command_format_permissions(_stat.st_mode, permissons_string);

        char size_string[32] = {0};

        ls_command_format_size(1, _stat.st_size, size_string);

        printf("%c%s %i %s %s %s %s\n",
               mode_char,
               permissons_string,
               _stat.st_nlink,
               simple_cli_get_user_name(),
               simple_cli_get_user_group(),
               size_string,
               dirent.name);

        ++i;
    }

    syscall_close(fd);

    return 0;
}

int mkdir_command(int argc, const char **argv)
{
    if (argc <= 1)
    {
        printf("Argument expected\n");
    }

    for (int i = 1; i < argc; ++i)
    {
        int ret = syscall_mkdir((char *)argv[i], 0777);

        if (ret < 0)
        {
            return ret;
        }
    }

    return 0;
}

int pwd_command(int argc, const char **argv)
{
    printf("%s\n", simple_cli_get_working_directory());

    return 0;
}

static int rm_command_remove_item(int recursive, const char *path);
static int rm_command_remove_directory(int recursive, const char *dir);

static int rm_command_remove_item(int recursive, const char *path)
{
    struct stat statbuf;
    syscall_lstat(path, (uintptr_t)&statbuf);

    if (S_ISDIR(statbuf.st_mode))
    {
        if (!recursive)
        {
            printf("[RM]: %s is a directory\n", path);
            return 1;
        }
        return rm_command_remove_directory(recursive, path);
    }

    syscall_unlink(path);

    return 0;
}

static int rm_command_remove_directory(int recursive, const char *dir)
{
    int fd = syscall_open(dir, O_DIRECTORY, O_RDWR);

    if (fd < 0)
    {
        printf("Could not open %s\n", dir);
        return fd;
    }

    struct dirent dirent;
    int i = 0;

    while (syscall_readdir(fd, i, &dirent) == 1)
    {
        if (!strcmp(dirent.name, ".") || !strcmp(dirent.name, ".."))
        {
            ++i;
            continue;
        }

        char tmp[128] = {0};

        sprintf(tmp, "%s/%s", dir, dirent.name);

        int status = rm_command_remove_item(recursive, tmp);

        if (status)
        {
            return status;
        }

        ++i;
    }

    syscall_close(fd);

    syscall_unlink(dir);

    return 0;
}

int rm_command(int argc, const char **argv)
{
    int recursive = 0;

    if (argc <= 1)
    {
        printf("Argument expected\n");
    }

    if (strcmp(argv[1], "-r") == 0)
    {
        recursive = 1;
    }

    for (int i = recursive ? 2 : 1; i < argc; ++i)
    {
        rm_command_remove_item(recursive, argv[i]);
    }

    return 0;
}

simple_cli_command_t _commands[] = {{.name = "test", .command = test_command},
                                    {.name = "cd", .command = cd_command},
                                    {.name = "ls", .command = ls_command},
                                    {.name = "mkdir", .command = mkdir_command},
                                    {.name = "pwd", .command = pwd_command},
                                    {.name = "rm", .command = rm_command},
                                    {.name = NULL, .command = NULL}};

int simple_cli_run_command(const char *name, int argc, const char **argv)
{
    if (!simple_cli_is_command(name))
    {
        return -1;
    }

    int i = 0;

    while (_commands[i].name != NULL)
    {
        if (strcmp(_commands[i].name, name) == 0)
        {
            return _commands[i].command(argc, argv);
        }

        ++i;
    }

    return -1;
}

int simple_cli_is_command(const char *name)
{
    if (!name)
    {
        return 0;
    }

    if (strcmp(name, "") == 0)
    {
        return 0;
    }

    int i = 0;

    while (_commands[i].name != NULL)
    {
        if (strcmp(_commands[i].name, name) == 0)
        {
            return 1;
        }

        ++i;
    }

    return 0;
}

int simple_cli_launch(char **args)
{
    pid_t pid;
    pid_t wpid;

    int status = 0;

    printf("Args before fork: %#016x\n", &args);

    phys_mem_dump_statistics();
    virt_mem_print_dir(process_get_current()->page_directory);

    pid = process_fork();

    //debug_print_process_tree();

    if (pid == 0)
    {
        printf("Args after fork(child): %#016x\n", &args);
        printf("Child Process: %i\n", process_get_pid());
        //exec_elf(args[0], simple_cli_get_token_count(args), args, NULL, 0);

        // exec_elf should not return

        process_exit(1337);
    }
    else if (pid < 0)
    {
        printf("[FORK] Error\n");
    }
    else
    {
        printf("Args after fork(parent): %#016x\n", &args);
        printf("Parent Process: %i\n", process_get_pid());
        do
        {
            wpid = waitpid(pid, &status, 0);
        } while (0);
    }

    printf("Child process exited with status code %i\n", (int64_t)status);

    return 1;
}

void simple_cli_init()
{
    while (1)
    {
        char *line = simple_cli_read_line();
        const char **args = simple_cli_split_line(line);
        //simple_cli_launch(args);

        if (simple_cli_is_command(args[0]))
        {
            long long int return_code = simple_cli_run_command(args[0],
                                                               simple_cli_get_token_count(args),
                                                               args);

            if (return_code)
            {
                printf("Command returned with code: %i\n", return_code);
            }
        }
        else
        {
            printf("Invalid command\n");
        }
        free(line);
        free(args);
    }
}

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