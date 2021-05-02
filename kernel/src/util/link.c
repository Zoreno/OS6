/**
 * @file link.c
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

#include <util/link.h>

void link_init(link_t *x)
{
    x->prev = x;
    x->next = x;
}

void link_after(link_t *a, link_t *x)
{
    link_t *p = a;
    link_t *n = a->next;

    n->prev = x;
    x->next = n;
    x->prev = p;
    p->next = x;
}

void link_before(link_t *a, link_t *x)
{
    link_t *p = a->prev;
    link_t *n = a;

    n->prev = x;
    x->next = n;
    x->prev = p;
    p->next = x;
}

void link_remove(link_t *x)
{
    link_t *p = x->prev;
    link_t *n = x->next;

    n->prev = p;
    p->next = n;
    x->next = 0;
    x->prev = 0;
}

void link_move_after(link_t *a, link_t *x)
{
    link_t *p = x->prev;
    link_t *n = x->next;

    n->prev = p;
    p->next = n;

    p = a;
    n = a->next;
    n->prev = x;
    x->next = n;
    x->prev = p;
    p->next = x;
}

void link_move_before(link_t *a, link_t *x)
{
    link_t *p = x->prev;
    link_t *n = x->next;

    n->prev = p;
    p->next = n;

    p = a->prev;
    n = a;
    n->prev = x;
    x->next = n;
    x->prev = p;
    p->next = x;
}

int link_is_empty(link_t *x)
{
    return (x->next == x) ? 1 : 0;
}

//=============================================================================
// End of file
//=============================================================================
