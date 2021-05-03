/**
 * @file gmtime.c
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
#include <stdlib.h>

//=============================================================================
// Local definitions
//=============================================================================

static const int SECONDS_PER_MINUTE = 60;
static const int SECONDS_PER_HOUR = 3600;
static const int SECONDS_PER_DAY = 86400;

static const int DAYS_IN_MONTH[] = 
{
    31, // Jan
    28, // Feb
    31, // Mar
    30, // Apr
    31, // May
    30, // Jun
    31, // Jul
    31, // Aug
    30, // Sep
    31, // Oct
    30, // Nov
    31  // Dec
};

//=============================================================================
// Private forward declations
//=============================================================================

/**
 * @brief Returns the number of days for a specific month.
 * 
 * @param month Index of month. January is 0.
 * @param year Year in four digit form.
 * 
 * @return int Days in the month.
 */
static int days_in_month(int month, int year);

/**
 * @brief Returns the number of days in a specific year.
 * 
 * @param year Year in four digit form.
 * 
 * @return int Days in the year.
 */
static int days_in_year(int year);

/**
 * @brief Checks if the year is leap year.
 * 
 * @param year Year in four digit form, eg 2021.
 * 
 * @return int Non-zero if leap-year.
 */
static int is_leap_year(int year);

/**
 * @brief Returns the number of seconds per minute.
 * 
 * 
 * @return int Seconds per minute.
 */
static int seconds_per_minute();

/**
 * @brief Returns the number of seconds per hour.
 * 
 * 
 * @return int Seconds per hour.
 */
static int seconds_per_hour();

/**
 * @brief Returns the number of seconds per day.
 * 
 * 
 * @return int Seconds per day.
 */
static int seconds_per_day();

/**
 * @brief Ensures that the value pointed to by @a var is in the range 
 * [ @a min, @a max [, taking from or adding to the @a carry variable.
 *
 * @param var Pointer to variable to 
 * @param min Minimum value (inclusive) in the range.
 * @param max Maximum value (exclusive) in the range.
 * @param carry Pointer to variable to add/remove from.
 *
 */
static void validate_range(int *var, int min, int max, int *carry);

/**
 * @brief Validates the tm struct, checking for overflows in the variables.
 * 
 * @param time_p Pointer to struct to validate.
 * 
 */
static void validate(struct tm *time_p);

//=============================================================================
// Private functions
//=============================================================================

static int days_in_month(int month, int year)
{
    int days = DAYS_IN_MONTH[month];

    if (month == 1 && is_leap_year(year))
    {
        days += 1;
    }

    return days;
}

static int days_in_year(int year)
{
    return is_leap_year(year) ? 366 : 365;
}

static int is_leap_year(int year)
{
    return ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0));
}

static int seconds_per_minute()
{
    return SECONDS_PER_MINUTE;
}

static int seconds_per_hour()
{
    return SECONDS_PER_HOUR;
}

static int seconds_per_day()
{
    return SECONDS_PER_DAY;
}

static void validate_range(int *var, int min, int max, int *carry)
{
    div_t res;

    if(!var || !carry)
    {
        // TODO: This should be some error of some kind.
        return;
    }

    if (*var < min || *var > (max - 1))
    {
        res = div(*var, max);
        *carry += res.quot;

        *var = res.rem;

        if (*var < 0)
        {
            *var += max;
            --(*carry);
        }
    }
}

static void validate(struct tm *time_p)
{
    validate_range(&time_p->tm_sec, 0, 60, &time_p->tm_min);
    validate_range(&time_p->tm_min, 0, 60, &time_p->tm_hour);
    validate_range(&time_p->tm_hour, 0, 24, &time_p->tm_mday);
    validate_range(&time_p->tm_mon, 0, 12, &time_p->tm_year);

    if (time_p->tm_mday <= 0)
    {
        while(time_p->tm_mday <= 0)
        {
            if (--time_p->tm_mon == -1)
            {
                time_p->tm_year--;
                time_p->tm_mon = 11;
            }
            time_p->tm_mday += days_in_month(time_p->tm_mon, time_p->tm_year);
        }
    }
    else
    {
        while (time_p->tm_mday > days_in_month(time_p->tm_mon, time_p->tm_year))
        {
            time_p->tm_mday -= days_in_month(time_p->tm_mon, time_p->tm_year);
            if (++time_p->tm_mon == 12)
            {
                time_p->tm_year++;
                time_p->tm_mon = 0;
            }
        }
    }
}

//=============================================================================
// Interface functions
//=============================================================================

time_t mktime(struct tm *time_p)
{
    time_t time = 0;
    long days = 0;
    int year;
    int is_dst = 0;

    validate(time_p);

    time += time_p->tm_sec + time_p->tm_min * seconds_per_minute() + 
        time_p->tm_hour * seconds_per_hour();

    return time;
}

//=============================================================================
// End of file
//=============================================================================
