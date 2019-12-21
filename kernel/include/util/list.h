/** \addtogroup kernel 
 *  @{
 */

/**
 * @file list.h
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

#ifndef _LIST_H
#define _LIST_H

#include <stdint.h>

/**
 * @brief List node used in the list
 * 
 * 
 */
typedef struct _list_node
{
    /**
     * @brief Pointer to the next element in the list.
     * 
     * 
     */
    struct _list_node *next;

    /**
     * @brief Pointer to the previous element in the list.
     * 
     * 
     */
    struct _list_node *prev;

    /**
     * @brief The payload that the node contains.
     * 
     * 
     */
    void *payload;
} list_node_t;

/**
 * @brief List data structure
 * 
 * 
 */
typedef struct
{
    /**
     * @brief Pointer to the first element in the list.
     * 
     * 
     */
    list_node_t *head;

    /**
     * @brief Pointer to the last element in the list.
     * 
     * 
     */
    list_node_t *tail;

    /**
     * @brief The number of elements in the list.
     * 
     * 
     */
    size_t length;
} list_t;

/**
 * @brief Destroys a list, freeing all inner nodes of the list.
 * 
 * @param list Pointer to list struct.
 * 
 */
void list_destroy(list_t *list);

/**
 * @brief Frees the memory allocated by the list.
 * 
 * @param list Pointer to the list struct.
 * 
 */
void list_free(list_t *list);

/**
 * @brief Appends a list node to the list.
 * 
 * @param list Pointer to list.
 * @param item Pointer to new list node.
 * 
 */
void list_append(list_t *list, list_node_t *item);

/**
 * @brief Inserting an item in the list. This will allocate a new list node.
 * 
 * @param list Pointer to list.
 * @param item Pointer to item to append.
 * 
 */
void list_insert(list_t *list, void *item);

/**
 * @brief Creates a new list.
 * 
 * 
 * @return Pointer to new list struct.
 */
list_t *list_create();

/**
 * @brief Finds an element in the list.
 * 
 * @param list Pointer to list.
 * @param value Value to find.
 * 
 * @return Pointer to list node if found, else NULL.
 */
list_node_t *list_find(list_t *list, void *value);

/**
 * @brief Removes an element from the list.
 * 
 * @param list Pointer to list.
 * @param index Index of element to be removed.
 * 
 */
void list_remove(list_t *list, size_t index);

/**
 * @brief Deletes a list node from the list.
 * 
 * @param list Pointer to list.
 * @param node Pointer to list node to remove.
 * 
 */
void list_delete(list_t *list, list_node_t *node);

/**
 * @brief Removes the tail of the list.
 * 
 * @param list Pointer to list.
 * 
 * @return Pointer to removed element.
 */
list_node_t *list_pop(list_t *list);

/**
 * @brief Removes the head of the list.
 * 
 * @param list Pointer to list.
 * 
 * @return Pointer to removed element.
 */
list_node_t *list_dequeue(list_t *list);

/**
 * @brief Copies the list.
 * 
 * @param list Pointer to list.
 * 
 * @return Pointer to new list.
 */
list_t *list_copy(list_t *list);

/**
 * @brief Merges two lists, appending each element from source to dest.
 * 
 * @param dest Destination list pointer.
 * @param source Source list pointer.
 * 
 */
void list_merge(list_t *dest, list_t *source);

#endif

/** @}*/

//=============================================================================
// End of file
//=============================================================================