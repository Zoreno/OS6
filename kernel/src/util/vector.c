/**
 * @file vector.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-21
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

#include <util/vector.h>

#include <debug/backtrace.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void vector_init(vector_t *v)
{
    v->data = NULL,
    v->size = 0;
    v->count = 0;
}

int vector_count(vector_t *v)
{
    return v->count;
}

void vector_add(vector_t *v, void *element)
{
    if (v->size == 0)
    {
        printf("[VECTOR] Allocating memory\n");

        v->size = 10;
        v->data = malloc(sizeof(void *) * v->size);

        if (!v->data)
        {
            printf("[VECTOR] Failed to allocate memory");
            backtrace();
            return;
        }

        memset(v->data, 0, sizeof(void *) * v->size);
    }

    if (v->size == v->count)
    {
        printf("[VECTOR] Reallocating memory\n");

        v->size *= 2;
        v->data = realloc(v->data, sizeof(void *) * v->size);

        if (!v->data)
        {
            printf("[VECTOR] Failed to reallocate memory");
            backtrace();
            return;
        }
    }

    printf("v->data: %#016x, v->size: %i, v->count: %i\n", v->data, v->size, v->count);

    v->data[v->count] = element;
    ++v->count;
}

void vector_set(vector_t *v, int index, void *element)
{
    if (index >= v->count)
    {
        return;
    }

    v->data[index] = element;
}

void *vector_get(vector_t *v, int index)
{
    if (index >= v->count)
    {
        return NULL;
    }

    return v->data[index];
}

void vector_delete(vector_t *v, int index)
{
    if (index >= v->count)
    {
        return;
    }

    v->data[index] = NULL;

    int i;
    int j;

    memmove(v->data + index + 1, v->data + index, v->count - index);

    v->count--;
}

void vector_free(vector_t *v)
{
    free(v->data);
}

//=============================================================================
// End of file
//=============================================================================