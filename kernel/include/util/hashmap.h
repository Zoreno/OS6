/**
 * @file hashmap.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-19
 * 
 * @brief Hashmap
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

#ifndef _HASHMAP_H
#define _HASHMAP_H

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include <util/list.h>

typedef unsigned long long int (*hashmap_hash_t)(void *key);
typedef long long int (*hashmap_comp_t)(void *a, void *b);
typedef void (*hashmap_free_t)(void *p);
typedef void *(*hashmap_dupe_t)(void *p);

typedef struct hashmap_entry
{
    char *key;
    void *value;
    struct hashmap_entry *next;
} hashmap_entry_t;

typedef struct hashmap
{
    hashmap_hash_t hash_func;
    hashmap_comp_t hash_comp;
    hashmap_dupe_t hash_key_dup;
    hashmap_free_t hash_key_free;
    hashmap_free_t hash_val_free;
    size_t size;
    hashmap_entry_t **entries;
} hashmap_t;

hashmap_t *hashmap_create(int size);
hashmap_t *hashmap_create_int(int size);
void *hashmap_set(hashmap_t *map, void *key, void *value);
void *hashmap_get(hashmap_t *map, void *key);
void *hashmap_remove(hashmap_t *map, void *key);
int hashmap_has(hashmap_t *map, void *key);
list_t *hashmap_keys(hashmap_t *map);
list_t *hashmap_values(hashmap_t *map);
void hashmap_free(hashmap_t *map);
int hashmap_is_empty(hashmap_t *map);

#endif