/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <sc/ash/util.h>

#include <limits.h>

TEST_CASE(util_int_len)
{
    check(sc_int_len(0) == 1);
    check(sc_int_len(1) == 1);
    check(sc_int_len(-1) == 2);
    check(sc_int_len(12) == 2);
    check(sc_int_len(123) == 3);
    check(sc_int_len(1234) == 4);
    check(sc_int_len(12345) == 5);
    check(sc_int_len(-12345) == 6);
    check(sc_int_len(123456) == 6);
    check(sc_int_len(1234567) == 7);
    check(sc_int_len(12345678) == 8);
    check(sc_int_len(123456789) == 9);
    check(sc_int_len(1234567890) == 10);
    check(sc_int_len(-1234567890) == 11);
    check(sc_int_len(INT_MIN) == 11);
    check(sc_int_len(INT_MAX) == 10);
}

TEST_CASE(util_long_len)
{
    check(sc_long_len(0L) == 1);
    check(sc_long_len(1L) == 1);
    check(sc_long_len(-1L) == 2);
    check(sc_long_len(12L) == 2);
    check(sc_long_len(123L) == 3);
    check(sc_long_len(1234L) == 4);
    check(sc_long_len(12345L) == 5);
    check(sc_long_len(-12345L) == 6);
    check(sc_long_len(123456L) == 6);
    check(sc_long_len(1234567L) == 7);
    check(sc_long_len(12345678L) == 8);
    check(sc_long_len(123456789L) == 9);
    check(sc_long_len(1234567890L) == 10);
    check(sc_long_len(-1234567890L) == 11);
    check(sc_long_len(12345678901L) == 11);
    check(sc_long_len(123456789012L) == 12);
    check(sc_long_len(1234567890123L) == 13);
    check(sc_long_len(12345678901234L) == 14);
    check(sc_long_len(123456789012345L) == 15);
    check(sc_long_len(-123456789012345L) == 16);
    check(sc_long_len(1234567890123456L) == 16);
    check(sc_long_len(12345678901234567L) == 17);
    check(sc_long_len(123456789012345678L) == 18);
    check(sc_long_len(1234567890123456789L) == 19);
    check(sc_long_len(LONG_MIN) == 20);
    check(sc_long_len(LONG_MAX) == 19);
}
