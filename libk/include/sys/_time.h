/**
 * @file _time.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-02-09
 * 
 * @brief Defines common time types for the kernel library.
 * 
 * @copyright Copyright (C) 2021,
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

#ifndef _LIBK__TIME_H
#define _LIBK__TIME_H

#include <__libk_common.h>

typedef long time_t;
typedef unsigned long clock_t;
typedef unsigned long timer_t;
typedef long susecond_t;
typedef unsigned long usecond_t;
struct tm
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

struct timespec
{
    time_t tv_sec;
    long tv_nsec;
};

struct timeval 
{
    time_t tv_sec;
    susecond_t tv_usec;
};

struct timezone
{
    int tz_min;
    int tz_dsttime;
};

struct itimerspec
{
    struct timespec it_interval;
    struct timespec it_value;
};

struct itimerval
{
    struct timeval it_interval;
    struct timeval it_value;
};

#endif

//=============================================================================
// End of file
//=============================================================================