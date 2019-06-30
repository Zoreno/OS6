#include <mm_alloc.h>

extern void *malloc(size_t);
extern void free(void *);

void *_mm_malloc(size_t __size, size_t __align)
{
    void *__malloc_ptr;
    void *__aligned_ptr;

    if (__align & (__align - 1))
    {
        return ((void *)0);
    }

    if (__size == 0)
    {
        return ((void *)0);
    }

    if (__align < 2 * sizeof(void *))
    {
        __align = 2 * sizeof(void *);
    }

    __malloc_ptr = malloc(__size + __align);

    if (!__malloc_ptr)
    {
        return ((void *)0);
    }

    __aligned_ptr = (void *)(((size_t)__malloc_ptr + __align) & ~((size_t)(__align)-1));

    ((void **)__aligned_ptr)[-1] = __malloc_ptr;

    return __aligned_ptr;
}

void _mm_free(void *__ptr)
{
    if (__ptr)
    {
        free(((void **)__ptr)[-1]);
    }
}