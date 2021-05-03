/**
 * @file huffman.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-02
 *
 * @brief Helpers for parsing huffman-coded data.
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

#include <util/huffman.h>

#include <stddef.h>
#include <stdint.h>

//=============================================================================
// Local definitons
//=============================================================================

typedef struct _huffman_table_t
{
    uint16_t counts[16];
    uint16_t symbols[288];
} huffman_table_t;

typedef struct _huffman_ring_t
{
    size_t pointer;
    uint8_t data[32768];
} huffman_ring_t;

#define GZIP_FLAG_TEXT (1 << 0)
#define GZIP_FLAG_HCRC (1 << 1)
#define GZIP_FLAG_EXTR (1 << 2)
#define GZIP_FLAG_NAME (1 << 3)
#define GZIP_FLAG_COMM (1 << 4)

//=============================================================================
// Local data
//=============================================================================

static huffman_table_t FIXED_LENGTH_TABLE;
static huffman_table_t FIXED_DISTS_TABLE;

static huffman_ring_t DATA = {0, {0}};

//=============================================================================
// Private function forward declarations
//=============================================================================

static uint16_t read_16le(huffman_context_t *context);
static uint16_t read_32le(huffman_context_t *context);
static uint8_t read_bit(huffman_context_t *context);
static uint32_t read_bits(huffman_context_t *context, uint32_t count);

static void build_huffman_table(uint8_t *lengths,
                                size_t size,
                                huffman_table_t *table);
static void build_fixed_tables();

static int decode(huffman_context_t *context, huffman_table_t *table);
static void emit(huffman_context_t *context, unsigned char byte);
static uint8_t peek(huffman_context_t *context, int offset);
static int inflate(huffman_context_t *context,
                   huffman_table_t *huff_len,
                   huffman_table_t *huff_dist);

static void decode_huffman(huffman_context_t *context);
static int uncompressed(huffman_context_t *context);

//=============================================================================
// Private function implementations
//=============================================================================

static uint16_t read_16le(huffman_context_t *context)
{
    uint16_t a;
    uint16_t b;
    a = context->read(context);
    b = context->read(context);

    return (b << 8) | (a << 0);
}

static uint16_t read_32le(huffman_context_t *context)
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    a = context->read(context);
    b = context->read(context);
    c = context->read(context);
    d = context->read(context);

    return (d << 24) | (c << 16) | (b << 8) | (a << 0);
}

static uint8_t read_bit(huffman_context_t *context)
{
    if (context->buffer_size == 0)
    {
        context->bit_buffer = context->read(context);
        context->buffer_size = 8;
    }

    int out = context->bit_buffer & 1;
    context->bit_buffer >>= 1;
    context->buffer_size -= 1;

    return out;
}

static uint32_t read_bits(huffman_context_t *context, uint32_t count)
{
    uint32_t out = 0;

    for (uint32_t bit = 0; bit < count; ++bit)
    {
        out |= (read_bit(context) << bit);
    }

    return out;
}

static void build_huffman_table(uint8_t *lengths,
                                size_t size,
                                huffman_table_t *table)
{
    uint16_t offsets[16];
    uint32_t count = 0;

    for (uint32_t i = 0; i < 16; ++i)
    {
        table->counts[i] = 0;
    }

    for (uint32_t i = 0; i < size; ++i)
    {
        table->counts[lengths[i]]++;
    }

    table->counts[0] = 0;

    for (uint32_t i = 0; i < 16; ++i)
    {
        offsets[i] = count;
        count += table->counts[i];
    }

    for (uint32_t i = 0; i < size; ++i)
    {
        if (lengths[i])
        {
            table->symbols[offsets[lengths[i]]++] = i;
        }
    }
}

static void build_fixed_tables()
{
    uint8_t lengths[288];

    int i = 0;

    for (i; i < 144; ++i)
    {
        lengths[i] = 8;
    }

    for (i; i < 256; ++i)
    {
        lengths[i] = 9;
    }

    for (i; i < 280; ++i)
    {
        lengths[i] = 7;
    }

    for (i; i < 288; ++i)
    {
        lengths[i] = 8;
    }

    build_huffman_table(lengths, 288, &FIXED_LENGTH_TABLE);

    for (int i = 0; i < 30; ++i)
    {
        lengths[i] = 5;
    }

    build_huffman_table(lengths, 30, &FIXED_DISTS_TABLE);
}

static int decode(huffman_context_t *context, huffman_table_t *table)
{
    int count = 0;
    int cur = 0;

    for (int i = 1; cur >= 0; ++i)
    {
        cur = (cur << 1) | read_bit(context);
        count += table->counts[i];
        cur -= table->counts[i];
    }

    return table->symbols[count + cur];
}

static void emit(huffman_context_t *context, unsigned char byte)
{
    if (context->ring->pointer >= 32768)
    {
        context->ring->pointer = 0;
    }

    context->ring->data[context->ring->pointer] = byte;
    context->write(context, byte);
    ++context->ring->pointer;
}

static uint8_t peek(huffman_context_t *context, int offset)
{
    return context->ring->data[(context->ring->pointer - offset) % 32768];
}

static int inflate(huffman_context_t *context,
                   huffman_table_t *huff_len,
                   huffman_table_t *huff_dist)
{
    static const uint16_t lens[] = {3,  4,  5,  6,   7,   8,   9,   10,  11, 13,
                                    15, 17, 19, 23,  27,  31,  35,  43,  51, 59,
                                    67, 83, 99, 115, 131, 163, 195, 227, 258};
    static const uint16_t lext[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2,
                                    2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};

    static const uint16_t dists[] = {
        1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
        33,   49,   65,   97,   129,  193,  257,  385,   513,   769,
        1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
    static const uint16_t dext[] = {0, 0, 0,  0,  1,  1,  2,  2,  3,  3,
                                    4, 4, 5,  5,  6,  6,  7,  7,  8,  8,
                                    9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

    while (1)
    {
        int symbol = decode(context, huff_len);

        if (symbol == 256)
        {
            // End of data.
            break;
        }

        if (symbol < 256)
        {
            emit(context, symbol);
        }
        else
        {
            int length;
            int distance;
            int offset;

            symbol -= 257;
            length = read_bits(context, lext[symbol]) + lens[symbol];
            distance = decode(context, huff_dist);
            offset = read_bits(context, dext[distance]) + dists[distance];

            for (int i = 0; i < length; ++i)
            {
                uint8_t b = peek(context, offset);
                emit(context, b);
            }
        }
    }

    return 0;
}

static void decode_huffman(huffman_context_t *context)
{
    static const uint8_t clens[] = {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    unsigned int literals;
    unsigned int distances;
    unsigned int clengths;
    uint8_t lengths[320] = {0};

    literals = 256 + read_bits(context, 5);
    distances = 1 + read_bits(context, 5);
    clengths = 4 + read_bits(context, 4);

    for (unsigned int i = 0; i < clengths; ++i)
    {
        lengths[clens[i]] = read_bits(context, 3);
    }

    huffman_table_t codes;
    build_huffman_table(lengths, 19, &codes);

    unsigned int count = 0;
    while (count < literals + distances)
    {
        int symbol = decode(context, &codes);

        if (symbol < 16)
        {
            lengths[count++] = symbol;
        }
        else if (symbol < 19)
        {
            int rep = 0;
            int length = 0;

            if (symbol == 16)
            {
                rep = lengths[count - 1];
                length = read_bits(context, 2) + 3;
            }
            else if (symbol == 17)
            {
                length = read_bits(context, 3) + 3;
            }
            else if (symbol == 18)
            {
                length = read_bits(context, 7) + 11;
            }

            do
            {
                lengths[count++] = rep;
                length--;
            } while (length);
        }
        else
        {
            break;
        }
    }

    huffman_table_t huff_len;
    build_huffman_table(lengths, literals, &huff_len);
    huffman_table_t huff_dist;
    build_huffman_table(lengths + literals, distances, &huff_dist);

    inflate(context, &huff_len, &huff_dist);
}

static int uncompressed(huffman_context_t *context)
{
    context->bit_buffer = 0;
    context->buffer_size = 0;

    uint16_t len = read_16le(context);
    uint16_t nlen = read_16le(context);

    if ((nlen & 0xFFFF) != (~len & 0xFFFF))
    {
        return 1;
    }

    for (int i = 0; i < len; ++i)
    {
        emit(context, context->read(context));
    }

    return 0;
}

//=============================================================================
// Interface functions
//=============================================================================

int deflate(huffman_context_t *context)
{
    context->bit_buffer = 0;
    context->buffer_size = 0;

    build_fixed_tables();

    if (!context->ring)
    {
        context->ring = &DATA;
    }

    while (1)
    {
        int is_final = read_bit(context);
        int type = read_bits(context, 2);

        switch (type)
        {
        case 0x00:
            uncompressed(context);
            break;
        case 0x01:
            inflate(context, &FIXED_LENGTH_TABLE, &FIXED_DISTS_TABLE);
            break;
        case 0x02:
            decode_huffman(context);
            break;
        case 0x03:
            return 1;

        default:
            break;
        }

        if (is_final)
        {
            break;
        }
    }

    return 0;
}

int gzip(huffman_context_t *context)
{
    if (context->read(context) != 0x1F)
    {
        return 1;
    }

    if (context->read(context) != 0x8B)
    {
        return 1;
    }

    unsigned int cm = context->read(context);

    if (cm != 8)
    {
        return 1;
    }

    unsigned int flags = context->read(context);

    unsigned int mtime = read_32le(context);
    (void)mtime;

    unsigned int xflags = context->read(context);
    (void)xflags;

    unsigned int os = context->read(context);
    (void)os;

    if (flags & GZIP_FLAG_EXTR)
    {
        uint16_t size = read_16le(context);

        for (uint16_t i = 0; i < size; ++i)
        {
            context->read(context);
        }
    }

    if (flags & GZIP_FLAG_NAME)
    {
        uint32_t c;

        // Read past the NULL-terminated string and ignore it.
        while ((c = context->read(context)) != 0)
            ;
    }

    if (flags & GZIP_FLAG_COMM)
    {
        uint32_t c;

        // Read past the NULL-terminated string and ignore it.
        while ((c = context->read(context)) != 0)
            ;
    }

    uint32_t crc16 = 0;

    if (flags & GZIP_FLAG_HCRC)
    {
        crc16 = read_16le(context);
    }

    int status = deflate(context);

    uint32_t crc32 = read_32le(context);
    uint32_t dsize = read_32le(context);

    (void)crc32;
    (void)dsize;

    return status;
}

//=============================================================================
// End of file
//=============================================================================
