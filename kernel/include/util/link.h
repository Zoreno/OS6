/**
 * @file link.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
 *
 * @brief
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

#ifndef _LINK_H
#define _LINK_H

#include <stdint.h>

typedef struct _link_t
{
    struct _link_t *prev;
    struct _link_t *next;
} link_t;

void link_init(link_t *x);
void link_after(link_t *a, link_t *x);
void link_before(link_t *a, link_t *x);
void link_remove(link_t *x);
void link_move_after(link_t *a, link_t *x);
void link_move_before(link_t *a, link_t *x);
int link_is_empty(link_t *x);

#define link_data(link, T, m) \
    (T *)((char *)(link) - (unsigned long long)(&(((T *)0)->m)))

#define list_for_each(it, list, m, T)                          \
    for (it = link_data((list).next, T, m); &it->m != &(list); \
         it = link_data(it->m.next, T, m))

#define list_for_each_safe(it, n, list, m, T)                                \
    for (it = link_data((list).next, T, m), n = link_data(it->m.next, T, m); \
         &it->m != &(list);                                                  \
         it = n, n = link_data(n->m.next, T, m))

#endif

//=============================================================================
// End of file
//=============================================================================
