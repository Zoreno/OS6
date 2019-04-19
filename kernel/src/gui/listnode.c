#include <gui/listnode.h>

#include <stdint.h>
#include <stdlib.h>

gui_list_node_t *gui_list_node_new(void *payload)
{
    gui_list_node_t *list_node;

    if (!(list_node = (gui_list_node_t *)malloc(sizeof(gui_list_node_t))))
        return list_node;

    list_node->prev = (gui_list_node_t *)0;
    list_node->next = (gui_list_node_t *)0;
    list_node->payload = payload;

    return list_node;
}
