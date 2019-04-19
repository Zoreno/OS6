#ifndef _LISTNODE_H
#define _LISTNODE_H

typedef struct _gui_list_node_t
{
    void *payload;
    struct _gui_list_node_t *prev;
    struct _gui_list_node_t *next;
} gui_list_node_t;

gui_list_node_t *gui_list_node_new(void *payload);

#endif