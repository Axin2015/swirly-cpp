/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include "IntWrapper.hpp"

#include <swirly/unit/Test.hpp>

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace swirly;

namespace {

struct TestTag : Int32Policy {
};
using Test = IntWrapper<TestTag>;

constexpr Test operator""_test(unsigned long long val) noexcept
{
    return Test{val};
}

} // swirly

SWIRLY_TEST_CASE(IntAddAssign)
{
    auto x = 5_test;
    SWIRLY_CHECK((x += 3_test) == 8_test);
    SWIRLY_CHECK(x == 8_test);
}

SWIRLY_TEST_CASE(IntSubAssign)
{
    auto x = 5_test;
    SWIRLY_CHECK((x -= 3_test) == 2_test);
    SWIRLY_CHECK(x == 2_test);
}

SWIRLY_TEST_CASE(IntMulAssign)
{
    auto x = 5_test;
    SWIRLY_CHECK((x *= 3_test) == 15_test);
    SWIRLY_CHECK(x == 15_test);
}

SWIRLY_TEST_CASE(IntDivAssign)
{
    auto x = 15_test;
    SWIRLY_CHECK((x /= 3_test) == 5_test);
    SWIRLY_CHECK(x == 5_test);
}

SWIRLY_TEST_CASE(IntModAssign)
{
    auto x = 5_test;
    SWIRLY_CHECK((x %= 3_test) == 2_test);
    SWIRLY_CHECK(x == 2_test);
}

SWIRLY_TEST_CASE(IntBitwiseAndAssign)
{
    // 11100
    // 00111
    // 00100
    auto x = 28_test;
    SWIRLY_CHECK((x &= 7_test) == 4_test);
    SWIRLY_CHECK(x == 4_test);
}

SWIRLY_TEST_CASE(IntBitwiseOrAssign)
{
    // 11100
    // 00111
    // 11111
    auto x = 28_test;
    SWIRLY_CHECK((x |= 7_test) == 31_test);
    SWIRLY_CHECK(x == 31_test);
}

SWIRLY_TEST_CASE(IntBitwiseXorAssign)
{
    // 11100
    // 00111
    // 11011
    auto x = 28_test;
    SWIRLY_CHECK((x ^= 7_test) == 27_test);
    SWIRLY_CHECK(x == 27_test);
}

SWIRLY_TEST_CASE(IntLeftShiftAssign)
{
    // 00111
    // 11100
    auto x = 7_test;
    SWIRLY_CHECK((x <<= 2_test) == 28_test);
    SWIRLY_CHECK(x == 28_test);
}

SWIRLY_TEST_CASE(IntRightShiftAssign)
{
    // 11100
    // 00111
    auto x = 28_test;
    SWIRLY_CHECK((x >>= 2_test) == 7_test);
    SWIRLY_CHECK(x == 7_test);
}

SWIRLY_TEST_CASE(IntPreInc)
{
    auto x = 5_test;
    SWIRLY_CHECK(++x == 6_test);
    SWIRLY_CHECK(x == 6_test);
}

SWIRLY_TEST_CASE(IntPreDec)
{
    auto x = 5_test;
    SWIRLY_CHECK(--x == 4_test);
    SWIRLY_CHECK(x == 4_test);
}

SWIRLY_TEST_CASE(IntPostInc)
{
    auto x = 5_test;
    SWIRLY_CHECK(x++ == 5_test);
    SWIRLY_CHECK(x == 6_test);
}

SWIRLY_TEST_CASE(IntPostDec)
{
    auto x = 5_test;
    SWIRLY_CHECK(x-- == 5_test);
    SWIRLY_CHECK(x == 4_test);
}

SWIRLY_TEST_CASE(IntUnaryPlus)
{
    constexpr auto x = 5_test;
    SWIRLY_CHECK(+x == 5_test);
}

SWIRLY_TEST_CASE(IntUnaryMinus)
{
    constexpr auto x = 5_test;
    SWIRLY_CHECK(-x == -5_test);
}

SWIRLY_TEST_CASE(IntAdd)
{
    constexpr auto x = 5_test;
    SWIRLY_CHECK((x + 3_test) == 8_test);
}

SWIRLY_TEST_CASE(IntSub)
{
    constexpr auto x = 5_test;
    SWIRLY_CHECK((x - 3_test) == 2_test);
}

SWIRLY_TEST_CASE(IntMul)
{
    constexpr auto x = 5_test;
    SWIRLY_CHECK((x * 3_test) == 15_test);
}

SWIRLY_TEST_CASE(IntDiv)
{
    constexpr auto x = 15_test;
    SWIRLY_CHECK(x / 3_test == 5_test);
}

SWIRLY_TEST_CASE(IntMod)
{
    constexpr auto x = 5_test;
    SWIRLY_CHECK((x % 3_test) == 2_test);
}

SWIRLY_TEST_CASE(IntBitwiseNot)
{
    constexpr auto x = Test{~1};
    SWIRLY_CHECK(~x == 1_test);
}

SWIRLY_TEST_CASE(IntBitwiseAnd)
{
    // 11100
    // 00111
    // 00100
    constexpr auto x = 28_test;
    SWIRLY_CHECK((x & 7_test) == 4_test);
}

SWIRLY_TEST_CASE(IntBitwiseOr)
{
    // 11100
    // 00111
    // 11111
    constexpr auto x = 28_test;
    SWIRLY_CHECK((x | 7_test) == 31_test);
}

SWIRLY_TEST_CASE(IntBitwiseXor)
{
    // 11100
    // 00111
    // 11011
    constexpr auto x = 28_test;
    SWIRLY_CHECK((x ^ 7_test) == 27_test);
}

SWIRLY_TEST_CASE(IntLeftShift)
{
    // 00111
    // 11100
    constexpr auto x = 7_test;
    SWIRLY_CHECK((x << 2_test) == 28_test);
}

SWIRLY_TEST_CASE(IntRightShift)
{
    // 11100
    // 00111
    constexpr auto x = 28_test;
    SWIRLY_CHECK((x >> 2_test) == 7_test);
}

SWIRLY_TEST_CASE(IntEqualTo)
{
    SWIRLY_CHECK(5_test == 5_test);
}

SWIRLY_TEST_CASE(IntNotEqualTo)
{
    SWIRLY_CHECK(5_test != 7_test);
}

SWIRLY_TEST_CASE(IntLessThan)
{
    SWIRLY_CHECK(5_test < 7_test);
}

SWIRLY_TEST_CASE(IntGreaterThan)
{
    SWIRLY_CHECK(5_test > 3_test);
}

SWIRLY_TEST_CASE(IntLessThanOrEqualTo)
{
    SWIRLY_CHECK(5_test <= 5_test && 5_test <= 7_test);
}

SWIRLY_TEST_CASE(IntGreaterThanOrEqualTo)
{
    SWIRLY_CHECK(5_test >= 5_test && 5_test >= 3_test);
}

SWIRLY_TEST_CASE(IntInsertion)
{
    SWIRLY_CHECK(lexical_cast<string>(5_test) == "5");
}
