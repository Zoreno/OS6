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

#ifndef _VECTOR_H
#define _VECTOR_H

typedef struct _vector
{
    void **data;
    int size;
    int count;
} vector_t;

void vector_init(vector_t *v);
int vector_count(vector_t *v);
void vector_add(vector_t *v, void *element);
void vector_set(vector_t *v, int index, void *element);
void *vector_get(vector_t *v, int index);
void vector_delete(vector_t *v, int index);
void vector_free(vector_t *v);

#endif

//=============================================================================
// End of file
//=============================================================================