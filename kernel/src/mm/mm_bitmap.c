#include <mm/mm_bitmap.h>

#define BITS_PER_ENTRY 64

void mm_bitmap_set(mm_bitmap_t bitmap, uint64_t bit)
{
    bitmap[bit / BITS_PER_ENTRY] |= (1 << (bit % BITS_PER_ENTRY));
}

void mm_bitmap_clear(mm_bitmap_t bitmap, uint64_t bit)
{
    bitmap[bit / BITS_PER_ENTRY] &= ~(1 << (bit % BITS_PER_ENTRY));
}

uint64_t mm_bitmap_test(mm_bitmap_t bitmap, uint64_t bit)
{
    return bitmap[bit / BITS_PER_ENTRY] & (1 << (bit % BITS_PER_ENTRY));
}

uint64_t mm_bitmap_first_free(mm_bitmap_t bitmap, size_t bitmap_size)
{
    for (uint64_t entry = 0; entry < bitmap_size / BITS_PER_ENTRY; ++entry)
    {
        if (bitmap[entry] == 0xFFFFFFFFFFFFFFFF)
        {
            continue;
        }

        for (size_t bit = 0; bit < BITS_PER_ENTRY; ++bit)
        {
            if (!(bitmap[entry] & (1 << bit)))
            {
                return entry * BITS_PER_ENTRY + bit;
            }
        }
    }

    return -1;
}

uint64_t mm_bitmap_first_free_s(mm_bitmap_t bitmap, size_t bitmap_size, size_t n)
{
    if (n == 0)
    {
        return -1;
    }

    uint64_t first_free = mm_bitmap_first_free(bitmap, bitmap_size);

    if ((int)first_free == -1)
    {
        return -1;
    }

    if (n == 1)
    {
        return first_free;
    }

    uint64_t contiguous_free = 0;

    uint64_t first_bit_in_chain = first_free;

    for (uint64_t bit = first_free; bit < bitmap_size; ++bit)
    {
        if (mm_bitmap_test(bitmap, bit))
        {
            contiguous_free = 0;
            first_bit_in_chain = bit + 1;
        }
        else
        {
            ++contiguous_free;
        }

        if (contiguous_free == n)
        {
            return first_bit_in_chain;
        }
    }

    return -1;
}