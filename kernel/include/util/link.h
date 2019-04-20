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

#define list_for_each(it, list, m, T)       \
    for (it = link_data((list).next, T, m); \
         &it->m != &(list);                 \
         it = link_data(it->m.next, T, m))

#define list_for_each_safe(it, n, list, m, T) \
    for (it = link_data((list).next, T, m),   \
        n = link_data(it->m.next, T, m);      \
         &it->m != &(list);                   \
         it = n,                              \
        n = link_data(n->m.next, T, m))

#endif