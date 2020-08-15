
#include <utility>

#include <bits/allocator.hpp>

#include <bit>

#include <bits/stl_heap.hpp>

void f()
{
    int i = 0;
    int j = 1;

    std::swap(i, j);

    int a = 14;

    int b = std::__rotl(a, 2);
}

// Remove this file when there are real cpp files to compile
