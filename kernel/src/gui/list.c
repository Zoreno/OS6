#include <gui/list.h>

#include <stdint.h>
#include <stdlib.h>

gui_list_t *gui_list_new()
{
    gui_list_t *list;

    if (!(list = (gui_list_t *)malloc(sizeof(gui_list_t))))
        return list;

    list->count = 0;
    list->root_node = (gui_list_node_t *)0;

    return list;
}

int gui_list_add(gui_list_t *list, void *payload)
{
    gui_list_node_t *new_node;
    if (!(new_node = gui_list_node_new(payload)))
        return 0;

    if (!list->root_node)
    {
        list->root_node = new_node;
    }
    else
    {
        gui_list_node_t *current_node = list->root_node;

        while (current_node->next)
        {
            current_node = current_node->next;
        }

        current_node->next = new_node;
        new_node->prev = current_node;
    }

    list->count++;

    return 1;
}

void *gui_list_get_at(gui_list_t *list, unsigned int index)
{
    if (list->count == 0 || index >= list->count)
    {
        return (void *)0;
    }

    gui_list_node_t *current_node = list->root_node;

    for (unsigned int current_index = 0; (current_index < index && current_node); ++current_index)
    {
        current_node = current_node->next;
    }

    return current_node ? current_node->payload : (void *)0;
}

void *gui_list_remove_at(gui_list_t *list, unsigned int index)
{
    void *payload;

    if (list->count == 0 || index >= list->count)
    {
        return (void *)0;
    }

    gui_list_node_t *current_node = list->root_node;

    for (unsigned int current_index = 0; (current_index < index) && current_node; current_index++)
    {
        current_node = current_node->next;
    }

    if (!current_node)
    {
        return (void *)0;
    }

    payload = current_node->payload;

    if (current_node->prev)
    {
        current_node->prev->next = current_node->next;
    }

    if (current_node->next)
    {
        current_node->next->prev = current_node->prev;
    }

    if (index == 0)
    {
        list->root_node = current_node->next;
    }

    free(current_node);

    list->count--;

    return payload;
}
