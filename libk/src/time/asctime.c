/**
 * @file asctime.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-02-16
 * 
 * @brief 
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

#include <stdio.h>
#include <time.h>

static const char day_name[7][3] = 
{
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"
};

static const char mon_name[12][3] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

static char result_buffer[32];

char *asctime(const struct tm *tp)
{
    sprintf(result_buffer, "%3s %3s%3d %2d:%2d:%2d %d\n",
        day_name[tp->tm_wday],
        mon_name[tp->tm_mon],
        tp->tm_mday,
        tp->tm_hour,
        tp->tm_min,
        tp->tm_sec,
        1900 + tp->tm_year);

    return result_buffer;
}

//=============================================================================
// End of file
//=============================================================================
