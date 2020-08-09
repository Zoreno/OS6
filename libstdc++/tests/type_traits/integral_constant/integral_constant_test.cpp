#include <type_traits>

#include <__test/__test.hpp>

#include <stdio.h>

typedef std::integral_constant<int, 1> ic_one;
typedef std::integral_constant<int, 0> ic_zero;
typedef std::integral_constant<int, -1> ic_minus_one;

typedef std::integral_constant<bool, true> ic_true;
typedef std::integral_constant<bool, false> ic_false;

static_assert(ic_one{}() == 1, "1");
static_assert(ic_zero{}() == 0, "0");
static_assert(ic_minus_one{}() == -1, "-1");

static_assert(ic_true{}() == true, "true");
static_assert(ic_false{}() == false, "false");

static_assert(ic_one() == 1, "");
static_assert(ic_zero() == 0, "");
static_assert(ic_minus_one() == -1, "");

static_assert(ic_true() == true, "");
static_assert(ic_false() == false, "");