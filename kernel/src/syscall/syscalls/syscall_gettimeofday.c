/**
 * @file syscall_gettimeofday.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-08
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

#include <syscall/syscall.h>

#include <cmos/cmos_rtc.h>
#include <logging/logging.h>

int syscall_gettimeofday(struct timeval *tv, struct timezone *tz)
{
    (void) tz; // TODO: Handle timezone.

    // TODO: Handle the case where RTC is not initialized correctly.

    ktime_t ktime;
    RTC_get_time(&ktime);

    time_t seconds = (time_t)RTC_time_to_int(&ktime);

    if (tv != NULL)
    {
        tv->tv_sec = seconds;
        tv->tv_usec = 0; // TODO: This needs to be filled out too.
    }

    log_info("gettimeofday: %i", seconds);

    return 0;
}

//=============================================================================
// End of file
//=============================================================================