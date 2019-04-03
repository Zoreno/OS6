#ifndef _LIST_H
#define _LIST_H

#include <stdint.h>

typedef struct _list_node
{
    struct _list_node *next;
    struct _list_node *prev;
    void *payload;
} list_node_t;

typedef struct
{
    list_node_t *head;
    list_node_t *tail;
    size_t length;
} list_t;

void list_destroy(list_t *list);
void list_free(list_t *list);
void list_append(list_t *list, list_node_t *item);
void list_insert(list_t *list, void *item);
list_t *list_create();
list_node_t *list_find(list_t *list, void *value);
void list_remove(list_t *list, size_t index);
void list_delete(list_t *list, list_node_t *node);
list_node_t *list_pop(list_t *list);
list_node_t *list_dequeue(list_t *list);
list_t *list_copy(list_t *list);
void list_merge(list_t *dest, list_t *source);

#endif