
#include <utility>

#include <bits/allocator.hpp>
#include <bits/alloc_traits.hpp>
#include <bits/stl_algobase.hpp>
#include <bits/stl_heap.hpp>
#include <bits/stl_construct.hpp>

#include <iterator>
#include <string>

void f()
{
    int a = 0;
    int b = 2;

    std::min(0, 2);
    std::max(0, 2);

    int arr[10] = {0};

    auto it = std::begin(arr);

    std::string str("Hello World");
    std::string str2;
    str2 = str;
    std::string str3(str, 2);
}

// Remove this file when there are real cpp files to compile
