/**
 * @file time.c
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

#include <time.h>
#include <sys/syscall.h>

time_t time(time_t *t)
{
    struct timeval now;

    if (_gettimeofday(&now, NULL) < 0)
    {
        now.tv_sec = (time_t)-1;
    }

    if (t)
    {
        *t = now.tv_sec;
    }

    return now.tv_sec;
}

//=============================================================================
// End of file
//=============================================================================
