#include <util/tree.h>

#include <stdlib.h>

tree_t *tree_create()
{
    tree_t *out = malloc(sizeof(tree_t));
    out->nodes = 0;
    out->root = NULL;

    return out;
}

void tree_set_root(tree_t *tree, void *value)
{
    tree_node_t *root = tree_node_create(value);
    tree->root = root;
    tree->nodes = 1;
}

void tree_node_destroy(tree_node_t *node)
{
    for (list_node_t *lnode = node->children->head; lnode != NULL; lnode = lnode->next)
    {
        tree_node_destroy((tree_node_t *)lnode->payload);
    }

    // This may be a memory leak
    free(node->value);
}

void tree_destroy(tree_t *tree)
{
    if (tree->root)
    {
        tree_node_destroy(tree->root);
    }
}

void tree_node_free(tree_node_t *node)
{
    if (!node)
    {
        return;
    }

    for (list_node_t *lnode = node->children->head; lnode != NULL; lnode = lnode->next)
    {
        tree_node_free((tree_node_t *)lnode->payload);
    }

    free(node);
}

void tree_free(tree_t *tree)
{
    tree_node_free(tree->root);
}

tree_node_t *tree_node_create(void *value)
{
    tree_node_t *out = malloc(sizeof(tree_node_t));
    out->value = value;
    out->children = list_create();
    out->parent = NULL;
    return out;
}

void tree_node_insert_child_node(tree_t *tree, tree_node_t *parent, tree_node_t *node)
{
    list_insert(parent->children, node);
    node->parent = parent;
    tree->nodes++;
}

tree_node_t *tree_node_insert_child(tree_t *tree, tree_node_t *parent, void *value)
{
    tree_node_t *out = tree_node_create(value);
    tree_node_insert_child_node(tree, parent, out);

    return out;
}

tree_node_t *tree_node_find_parent(tree_node_t *haystack, tree_node_t *needle)
{
    tree_node_t *found = NULL;

    for (list_node_t *lnode = haystack->children->head; lnode != NULL; lnode = lnode->next)
    {
        if (lnode->payload == needle)
        {
            return haystack;
        }

        found = tree_node_find_parent((tree_node_t *)lnode->payload, needle);

        if (found)
        {
            break;
        }
    }

    return found;
}

size_t tree_count_children(tree_node_t *node)
{
    if (!node)
    {
        return 0;
    }

    if (!node->children)
    {
        return 0;
    }

    size_t out = node->children->length;

    for (list_node_t *lnode = node->children->head; lnode != NULL; lnode = lnode->next)
    {
        out += tree_count_children((tree_node_t *)lnode->payload);
    }

    return out;
}

void tree_node_parent_remove(tree_t *tree, tree_node_t *parent, tree_node_t *node)
{
    tree->nodes -= tree_count_children(node) + 1;
    list_delete(parent->children, list_find(parent->children, node));
    tree_node_free(node);
}

void tree_node_remove(tree_t *tree, tree_node_t *node)
{
    tree_node_t *parent = node->parent;

    if (!parent)
    {
        if (node == tree->root)
        {
            tree->nodes = 0;
            tree->root = NULL;
            tree_node_free(node);
        }
    }

    tree_node_parent_remove(tree, parent, node);
}

void tree_remove(tree_t *tree, tree_node_t *node)
{
    tree_node_t *parent = node->parent;

    if (!parent)
    {
        return;
    }

    tree->nodes--;
    list_delete(parent->children, list_find(parent->children, node));
    for (list_node_t *lnode = node->children->head; lnode != NULL; lnode = lnode->next)
    {
        ((tree_node_t *)lnode->payload)->parent = parent;
    }

    list_merge(parent->children, node->children);
}

void tree_break_off(tree_t *tree, tree_node_t *node)
{
    (void)tree;

    tree_node_t *parent = node->parent;

    if (!parent)
    {
        return;
    }

    list_delete(parent->children, list_find(parent->children, node));
}

tree_node_t *tree_node_find(tree_node_t *node, void *search, tree_comparator_t comparator)
{
    if (comparator(node->value, search))
    {
        return node;
    }

    tree_node_t *found;

    for (list_node_t *lnode = node->children->head; lnode != NULL; lnode = lnode->next)
    {
        found = tree_node_find((tree_node_t *)lnode->payload, search, comparator);

        if (!found)
        {
            return found;
        }
    }

    return NULL;
}

tree_node_t *tree_find(tree_t *tree, void *value, tree_comparator_t comparator)
{
    return tree_node_find(tree->root, value, comparator);
}
