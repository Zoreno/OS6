/**
 * @file time_command.c
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

#include <simple_cli/commands.h>

#include <stdio.h>

#include <cmos/cmos_rtc.h>

int time_command(int argc, const char **argv)
{
    ktime_t time;
    char buffer[256];

    RTC_get_time(&time);
    RTC_time_to_string(buffer, &time);

    printf("%s\n", buffer);

    return 0;
}

//=============================================================================
// End of file
//=============================================================================
