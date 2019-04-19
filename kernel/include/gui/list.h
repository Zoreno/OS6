#ifndef _LIST_H_
#define _LIST_H_

#include <gui/listnode.h>

typedef struct _gui_list_t
{
    unsigned int count;
    gui_list_node_t *root_node;
} gui_list_t;

gui_list_t *gui_list_new();
int gui_list_add(gui_list_t *list, void *payload);
void *gui_list_get_at(gui_list_t *list, unsigned int index);
void *gui_list_remove_at(gui_list_t *list, unsigned int index);

#endif // _LIST_H_