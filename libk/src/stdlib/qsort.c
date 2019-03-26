#include <stdlib.h>
#include <stdint.h>

typedef int (*COMPARE)(const void *, const void *);

static void swap(void *mem1, void *mem2, size_t size);
static size_t partition(void *arr, size_t low, size_t high, size_t item_size,
                        COMPARE compare);
static void quicksort(void *arr, size_t low, size_t high, size_t item_size,
                      COMPARE compare);

static void swap(void *mem1, void *mem2, size_t size)
{
    uint8_t *b1 = (uint8_t *)mem1;
    uint8_t *b2 = (uint8_t *)mem2;

    if (b1 == b2)
    {
        return;
    }

    while (size--)
    {
        uint8_t b = *b1;
        *b1++ = *b2;
        *b2++ = b;
    }
}

static size_t partition(void *arr, size_t low, size_t high, size_t item_size,
                        COMPARE compare)
{
    int i = low - 1;

    uint8_t *arr_b = arr;

    for (size_t j = low; j < high; ++j)
    {
        if (compare((void *)(arr_b + j * item_size), (void *)(arr_b + high * item_size)))
        {
            ++i;
            swap(arr_b + i * item_size, arr_b + j * item_size, item_size);
        }

        swap(arr_b + (i + 1) * item_size, arr_b + high * item_size, item_size);
    }
    return i + 1;
}

static void quicksort(void *arr, size_t low, size_t high, size_t item_size,
                      COMPARE compare)
{
    if (low < high)
    {
        size_t partition_index = partition(arr, low, high, item_size, compare);

        quicksort(arr, low, partition_index - 1, item_size, compare);
        quicksort(arr, partition_index + 1, high, item_size, compare);
    }
}

void qsort(void *base, size_t nitems, size_t size, COMPARE compare)
{
    quicksort(base, 0, nitems - 1, size, compare);
}