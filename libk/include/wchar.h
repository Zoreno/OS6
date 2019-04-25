/**
 * @file wchar.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-25
 * 
 * @brief Functions working on wide strings
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

#ifndef _LIBK_WCHAR_H
#define _LIBK_WCHAR_H

#include <stddef.h>

int wcwidth(wchar_t c);

wchar_t *wcsncpy(wchar_t *dest, const wchar_t *src, size_t n);

size_t wcslen(const wchar_t *s);

int wcscmp(const wchar_t *s1, const wchar_t *s2);

wchar_t *wcscat(wchar_t *dest, const wchar_t *src);

wchar_t *wcstok(wchar_t *str, const wchar_t *delim, wchar_t **saveptr);

size_t wcsspn(const wchar_t *wcs, const wchar_t *accept);

wchar_t *wcspbrk(const wchar_t *wcs, const wchar_t *accept);

wchar_t *wcschr(const wchar_t *wcs, wchar_t wc);

wchar_t *wcsrchr(const wchar_t *wcs, wchar_t wc);

wchar_t *wcsncat(wchar_t *dest, const wchar_t *src, size_t n);

#endif