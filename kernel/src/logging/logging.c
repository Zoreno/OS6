/**
 * @file logging.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-30
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2020,
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

#include <logging/logging.h>

#include <arch/arch.h>
#include <cmos/cmos_rtc.h>

//=============================================================================
// Local variables
//=============================================================================

static const char *level_strings[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"};

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {
    "\x1b[94m",
    "\x1b[36m",
    "\x1b[32m",
    "\x1b[33m",
    "\x1b[31m",
    "\x1b[35m"};
#endif

static log_level_t level_threshold;

//=============================================================================
// Local functions
//=============================================================================

static int get_current_time(char *buffer, size_t buffer_size)
{
    (void)buffer_size;

    ktime_t time;

    RTC_get_time(&time);
    RTC_time_to_string(buffer, &time);

    return 0;
}

//=============================================================================
// Interface functions
//=============================================================================

void log_printf(log_level_t level,
                const char *file,
                int line,
                const char *fmt,
                ...)
{
    if (level < level_threshold)
    {
        return;
    }

    int prev_int_status = is_interrupts_enabled();

    cli();

    char buf[32];
    char str_buf[1024];

    get_current_time(buf, sizeof(buf));

#ifdef LOG_USE_COLOR
    printf("%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
           buf, level_colors[level], level_strings[level], file, line);
#else
    printf("%s %-5s %s:%d: ",
           buf, level_strings[level], file, line);
#endif
    va_list ap;
    va_start(ap, fmt);
    vsprintf(str_buf, fmt, ap);
    va_end(ap);

    printf("%s\n", str_buf);

    if (prev_int_status)
    {
        sti();
    }
}

const char *log_level_string(log_level_t level)
{
    return level_strings[level];
}

void log_set_threshold(log_level_t level)
{
    level_threshold = level;
}

log_level_t log_get_threshold()
{
    return level_threshold;
}

void log_init(log_level_t log_level)
{
    level_threshold = log_level;

    log_info("[LOG] Installed logging driver!");
}

//=============================================================================
// End of file
//=============================================================================