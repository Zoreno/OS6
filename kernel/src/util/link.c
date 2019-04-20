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