/**
 * @file huffman.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-02
 *
 * @brief Helper for parsing huffman-coded data.
 *
 * @copyright Copyright (C) 2021,
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

#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include <stdint.h>

struct _huffman_ring_t;

typedef struct _huffman_context_t
{
    void *input;
    void *output;

    uint8_t (*read)(struct _huffman_context_t *context);
    void (*write)(struct _huffman_context_t *context, unsigned int sym);

    int bit_buffer;
    int buffer_size;

    struct _huffman_ring_t *ring;

} huffman_context_t;

int deflate(huffman_context_t *context);
int gzip(huffman_context_t *context);

#endif

//=============================================================================
// End of file
//=============================================================================
