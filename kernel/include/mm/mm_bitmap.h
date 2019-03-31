#ifndef _MM_BITMAP_H
#define _MM_BITMAP_H

#include <stdint.h>

typedef uint64_t *mm_bitmap_t;

void mm_bitmap_set(mm_bitmap_t bitmap, uint64_t bit);
void mm_bitmap_clear(mm_bitmap_t bitmap, uint64_t bit);
uint64_t mm_bitmap_test(mm_bitmap_t bitmap, uint64_t bit);
uint64_t mm_bitmap_first_free(mm_bitmap_t bitmap, size_t bitmap_size);
uint64_t mm_bitmap_first_free_s(mm_bitmap_t bitmap, size_t bitmap_size, size_t n);

#endif