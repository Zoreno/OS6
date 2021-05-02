/**
 * @file cmos_rtc.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-07
 *
 * @brief Driver for the Real-time clock
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

#ifndef _CMOS_RTC_H
#define _CMOS_RTC_H

#include <stdint.h>

/**
 * @brief Struct representing a time stamp
 *
 *
 */
typedef struct
{
    /**
     * @brief Amount of seconds
     *
     *
     */
    uint8_t second;

    /**
     * @brief Amount of minutes
     *
     *
     */
    uint8_t minute;

    /**
     * @brief Amount of hours
     *
     *
     */
    uint8_t hour;

    /**
     * @brief Amount of days
     *
     *
     */
    uint8_t day;

    /**
     * @brief Amount of months
     *
     *
     */
    uint8_t month;

    /**
     * @brief Amount of years
     *
     *
     */
    uint32_t year;

} ktime_t;

/**
 * @brief Initiates the real-time clock
 *
 *
 */
void RTC_init();

/**
 * @brief Gets the current time
 *
 * @param time Pointer to struct where the time should be stored
 *
 */
void RTC_get_time(ktime_t *time);

/**
 * @brief Sets the current time
 *
 * @param time Pointer to struct containing time to set the clock to.
 *
 */
void RTC_set_time(ktime_t *time);

/**
 * @brief Minimum number of chars needed to safely store time string.
 *
 *
 */
#define RTC_TIME_STR_MINLEN 23

/**
 * @brief Converts the given time to string.
 *
 * Format: hh::mm::ss yyyy-mm-dd
 *
 * @param str Pointer to string to store the value.
 * @param time Time pointer to convert. If NULL, the current time is read and
 * used.
 *
 */
void RTC_time_to_string(char *str, ktime_t *time);

/**
 * @brief Converts the time to an integer
 *
 * @param time Time to convert
 *
 * @return uint32_t Seconds since 1 january 1970
 */
uint32_t RTC_time_to_int(ktime_t *time);

/**
 * @brief Converts an integer to a time struct
 *
 * @param time_int Time to convert
 * @param time Struct to store the result
 *
 */
void RTC_int_to_time(uint32_t time_int, ktime_t *time);

#endif

//=============================================================================
// End of file
//=============================================================================
