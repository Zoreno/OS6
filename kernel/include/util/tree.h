/**
 * @file tree.h
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

#ifndef _TREE_H
#define _TREE_H

#include <util/list.h>

#include <stdint.h>

typedef struct tree_node
{
    void *value;
    list_t *children;
    struct tree_node *parent;
} tree_node_t;

typedef struct
{
    size_t nodes;
    tree_node_t *root;
} tree_t;

typedef uint8_t (*tree_comparator_t)(void *, void *);

tree_t *tree_create();
int tree_set_root(tree_t *tree, void *value);
void tree_node_destroy(tree_node_t *node);
void tree_destroy(tree_t *tree);
void tree_free(tree_t *tree);
tree_node_t *tree_node_create(void *value);
void tree_node_insert_child_node(tree_t *tree,
                                 tree_node_t *parent,
                                 tree_node_t *node);
tree_node_t *tree_node_insert_child(tree_t *tree,
                                    tree_node_t *parent,
                                    void *value);
tree_node_t *tree_node_find_parent(tree_node_t *haystack, tree_node_t *needle);
size_t tree_count_children(tree_node_t *node);
void tree_node_parent_remove(tree_t *tree,
                             tree_node_t *parent,
                             tree_node_t *node);
void tree_node_remove(tree_t *tree, tree_node_t *node);
void tree_remove(tree_t *tree, tree_node_t *node);
void tree_remove_reparent_root(tree_t *tree, tree_node_t *node);
tree_node_t *tree_find(tree_t *tree, void *value, tree_comparator_t comparator);
void tree_break_off(tree_t *tree, tree_node_t *node);

#endif

//=============================================================================
// End of file
//=============================================================================
