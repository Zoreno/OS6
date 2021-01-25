/**
 * @file list.c
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

#include <util/list.h>

#include <assert.h>
#include <stdlib.h>

void list_destroy(list_t *list)
{
    ASSERT(list != NULL);

    list_node_t *n = list->head;

    while (n)
    {
        free(n->payload);
        n = n->next;
    }
}

void list_free(list_t *list)
{
    ASSERT(list != NULL);

    list_node_t *n = list->head;

    while (n)
    {
        list_node_t *s = n->next;
        free(n);
        n = s;
    }
}

void list_append(list_t *list, list_node_t *item)
{
    ASSERT(list != NULL);
    ASSERT(item != NULL);

    item->next = NULL;

    if (!list->tail)
    {
        list->head = item;
    }
    else
    {
        list->tail->next = item;
        item->prev = list->tail;
    }

    list->tail = item;
    ++list->length;
}

int list_insert(list_t *list, void *item)
{
    list_node_t *node = malloc(sizeof(list_node_t));

    if (!node)
    {
        return 0;
    }

    node->payload = item;
    node->next = NULL;
    node->prev = NULL;

    list_append(list, node);

    return 1;
}

list_t *list_create()
{
    list_t *list = malloc(sizeof(list_t));

    if (!list)
    {
        return list;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;

    return list;
}

list_node_t *list_find(list_t *list, void *value)
{
    ASSERT(list != NULL);

    for (list_node_t *item = list->head; item != NULL; item = item->next)
    {
        if (item->payload == value)
        {
            return item;
        }
    }

    return NULL;
}

list_node_t *list_find_comp(list_t *list, void *value, list_comp_t func)
{
    ASSERT(list != NULL);

    for (list_node_t *item = list->head; item != NULL; item = item->next)
    {
        if (func(item->payload, value))
        {
            return item;
        }
    }

    return NULL;
}

void list_remove(list_t *list, size_t index)
{
    ASSERT(list != NULL);

    if (index > list->length)
    {
        return;
    }

    list_node_t *n = list->head;

    for (size_t i = 0; i < index; ++i)
    {
        n = n->next;
    }

    list_delete(list, n);
}

void list_delete(list_t *list, list_node_t *node)
{
    ASSERT(list != NULL);
    ASSERT(node != NULL);

    if (node == list->head)
    {
        list->head = node->next;
    }

    if (node == list->tail)
    {
        list->tail = node->prev;
    }

    if (node->prev)
    {
        node->prev->next = node->next;
    }

    if (node->next)
    {
        node->next->prev = node->prev;
    }

    --list->length;
}

list_node_t *list_pop(list_t *list)
{
    ASSERT(list != NULL);

    if (!list->tail)
    {
        return NULL;
    }

    list_node_t *out = list->tail;

    list_delete(list, list->tail);

    return out;
}

list_node_t *list_dequeue(list_t *list)
{
    ASSERT(list != NULL);

    if (!list->head)
    {
        return NULL;
    }

    list_node_t *out = list->head;

    list_delete(list, list->head);

    return out;
}

list_t *list_copy(list_t *list)
{
    ASSERT(list != NULL);

    list_t *out = list_create();

    if (!out)
    {
        return out;
    }

    list_node_t *node = list->head;

    while (node)
    {
        int ret = list_insert(out, node->payload);

        ASSERT(ret != 0);
    }

    return out;
}

void list_merge(list_t *dest, list_t *source)
{
    ASSERT(dest != NULL);
    ASSERT(source != NULL);

    if (dest->tail)
    {
        dest->tail->next = source->head;
    }
    else
    {
        dest->head = source->head;
    }

    if (source->tail)
    {
        dest->tail = source->tail;
    }

    dest->length += source->length;

    free(source);
}

//=============================================================================
// End of file
//=============================================================================