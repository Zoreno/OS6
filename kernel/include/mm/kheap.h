#ifndef _KHEAP_H
#define _KHEAP_H

#include <stdint.h>

#define KHEAP_ALIGNMENT_MASK 0x00FFFFFFFFFFFFFF

#define BIT(i) (1 << (i))

#define HEAP_WITHIN_PAGE BIT(56)
#define HEAP_WITHIN_64K BIT(57)
#define HEAP_CONTINUOUS BIT(63)

void *kheap_get_current_end();
void kheap_init();
void *kmalloc(size_t size);
void *kmalloc_a(size_t size, uint64_t alignment);
void kfree(void *addr);

#endif