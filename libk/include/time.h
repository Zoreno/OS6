/**
 * @file time.h
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

#ifndef _LIBK_TIME_H
#define _LIBK_TIME_H

#include <__libk_common.h>

#include <sys/_time.h>

#include <stddef.h>

clock_t clock();
double difftime(time_t time1, time_t time2);
time_t mktime(struct tm *tp);
time_t time(time_t *time);

char *asctime(const struct tm *tp);
char *ctime(const time_t *time);
struct tm *gmtime(const time_t *time);
struct tm *localtime(const time_t *time);

size_t strftime(char *dst, size_t max_size, const char *format, const struct tm *time);
size_t strftime_l(char *dst, size_t max_size, const char *format, const struct tm *time, locale_t *locale);

char *strptime(const char *s, const char *format, struct tm *time);
char *strptime_l(const char *s, const char *format, struct tm *time, locale_t *locale);

#endif

//=============================================================================
// End of file
//=============================================================================