/**
 * @file hashmap.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-19
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

#include <util/hashmap.h>
#include <logging/logging.h>

//=============================================================================
// Static functions
//=============================================================================

static unsigned long long int hashmap_string_hash(void *_key)
{
    unsigned long long int hash = 0;
    char *key = (char *)_key;
    int c;

    while ((c = *key++))
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

static long long int hashmap_string_comp(void *a, void *b)
{
    return !strcmp(a, b);
}

void *hashmap_string_dupe(void *key)
{
    return strdup(key);
}

unsigned long long int hashmap_int_hash(void *key)
{
    return (unsigned long long int)key;
}

long long int hashmap_int_comp(void *a, void *b)
{
    return (long long int)a == (long long int)b;
}

void *hashmap_int_dupe(void *key)
{
    return key;
}

static void hashmap_int_free(void *ptr)
{
    (void)ptr;
    return;
}

//=============================================================================
// Interface functions
//=============================================================================

hashmap_t *hashmap_create(int size)
{
    hashmap_t *map = malloc(sizeof(hashmap_t));

    if (!map)
    {
        log_error("[HASHMAP] Failed to allocate memory for hashmap");
        return NULL;
    }

    map->hash_func = &hashmap_string_hash;
    map->hash_comp = &hashmap_string_comp;
    map->hash_key_dup = &hashmap_string_dupe;
    map->hash_key_free = &free;
    map->hash_val_free = &free;

    map->size = size;
    map->entries = malloc(sizeof(hashmap_entry_t *) * size);

    if (!map->entries)
    {
        log_error("[HASHMAP] Failed to allocate memory for hashmap");
        free(map);
        return NULL;
    }

    memset(map->entries, 0x00, sizeof(hashmap_entry_t *) * size);

    return map;
}

hashmap_t *hashmap_create_int(int size)
{
    hashmap_t *map = malloc(sizeof(hashmap_t));

    if (!map)
    {
        log_error("[HASHMAP] Failed to allocate memory for hashmap");
        return NULL;
    }

    map->hash_func = &hashmap_int_hash;
    map->hash_comp = &hashmap_int_comp;
    map->hash_key_dup = &hashmap_int_dupe;
    map->hash_key_free = &hashmap_int_free;
    map->hash_val_free = &free;

    map->size = size;
    map->entries = malloc(sizeof(hashmap_entry_t *) * size);

    if (!map->entries)
    {
        log_error("[HASHMAP] Failed to allocate memory for hashmap");
        free(map);
        return NULL;
    }

    memset(map->entries, 0x00, sizeof(hashmap_entry_t *) * size);

    return map;
}

void *hashmap_set(hashmap_t *map, void *key, void *value)
{
    unsigned int hash = map->hash_func(key) % map->size;

    hashmap_entry_t *x = map->entries[hash];

    if (!x)
    {
        hashmap_entry_t *e = malloc(sizeof(hashmap_entry_t));

        if (!e)
        {
            log_error("[HASHMAP] Failed to allocate memory for hashmap");
            return NULL;
        }

        e->key = map->hash_key_dup(key);
        e->value = value;
        e->next = NULL;
        map->entries[hash] = e;
        return NULL;
    }

    hashmap_entry_t *p = NULL;
    do
    {
        if (map->hash_comp(x->key, key))
        {
            void *out = x->value;
            x->value = value;
            return out;
        }
        else
        {
            p = x;
            x = x->next;
        }
    } while (x);

    hashmap_entry_t *e = malloc(sizeof(hashmap_entry_t));

    if (!e)
    {
        log_error("[HASHMAP] Failed to allocate memory for hashmap");
        return NULL;
    }

    e->key = map->hash_key_dup(key);
    e->value = value;
    e->next = NULL;

    p->next = e;

    return NULL;

}

void *hashmap_get(hashmap_t *map, void *key)
{
    unsigned int hash = map->hash_func(key) % map->size;

    hashmap_entry_t *x = map->entries[hash];

    if (!x)
    {
        return NULL;
    }
    else
    {
        do
        {
            if (map->hash_comp(x->key, key))
            {
                return x->value;
            }
            x = x->next;

        } while (x);

        return NULL;
    }
}

void *hashmap_remove(hashmap_t *map, void *key)
{
    unsigned int hash = map->hash_func(key) % map->size;

    hashmap_entry_t *x = map->entries[hash];

    if (!x)
    {
        return NULL;
    }
    else
    {
        if (map->hash_comp(x->key, key))
        {
            void *out = x->value;
            map->entries[hash] = x->next;
            map->hash_key_free(x->key);
            map->hash_val_free(x);
            return out;
        }
        else
        {
            hashmap_entry_t *p = x;
            x = x->next;

            do
            {
                if (map->hash_comp(x->key, key))
                {
                    void *out = x->value;
                    p->next = x->next;
                    map->hash_key_free(x->key);
                    map->hash_val_free(x);
                    return out;
                }

                p = x;
                x = x->next;

            } while (x);
        }

        return NULL;
    }
}

int hashmap_has(hashmap_t *map, void *key)
{
    unsigned int hash = map->hash_func(key) % map->size;

    hashmap_entry_t *x = map->entries[hash];

    if (!x)
    {
        return 0;
    }
    else
    {
        do
        {
            if (map->hash_comp(x->key, key))
            {
                return 1;
            }

            x = x->next;

        } while (x);

        return 0;
    }
}

list_t *hashmap_keys(hashmap_t *map)
{
    list_t *l = list_create();

    for (unsigned int i = 0; i < map->size; ++i)
    {
        hashmap_entry_t *x = map->entries[i];

        while (x)
        {
            list_insert(l, x->key);
            x = x->next;
        }
    }

    return l;
}

list_t *hashmap_values(hashmap_t *map)
{
    list_t *l = list_create();

    for (unsigned int i = 0; i < map->size; ++i)
    {
        hashmap_entry_t *x = map->entries[i];

        while (x)
        {
            list_insert(l, x->value);
            x = x->next;
        }
    }

    return l;
}

void hashmap_free(hashmap_t *map)
{
    for (unsigned int i = 0; i < map->size; ++i)
    {
        hashmap_entry_t *x = map->entries[i], *p;

        while (x)
        {
            p = x;
            x = x->next;
            map->hash_key_free(p->key);
            map->hash_val_free(p);
        }
    }

    free(map->entries);
}

int hashmap_is_empty(hashmap_t *map)
{
    for (unsigned int i = 0; i < map->size; ++i)
    {
        if (map->entries[i])
        {
            return 0;
        }
    }

    return 1;
}

//=============================================================================
// End of file
//=============================================================================
