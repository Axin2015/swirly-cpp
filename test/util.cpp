/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#include "test.hpp"

#include <dbr/util.h>

#include <limits.h>

TEST_CASE(util_int_len)
{
    check(dbr_int_len(0) == 1);
    check(dbr_int_len(1) == 1);
    check(dbr_int_len(-1) == 2);
    check(dbr_int_len(12) == 2);
    check(dbr_int_len(123) == 3);
    check(dbr_int_len(1234) == 4);
    check(dbr_int_len(12345) == 5);
    check(dbr_int_len(-12345) == 6);
    check(dbr_int_len(123456) == 6);
    check(dbr_int_len(1234567) == 7);
    check(dbr_int_len(12345678) == 8);
    check(dbr_int_len(123456789) == 9);
    check(dbr_int_len(1234567890) == 10);
    check(dbr_int_len(-1234567890) == 11);
    check(dbr_int_len(INT_MIN) == 11);
    check(dbr_int_len(INT_MAX) == 10);
}

TEST_CASE(util_long_len)
{
    check(dbr_long_len(0L) == 1);
    check(dbr_long_len(1L) == 1);
    check(dbr_long_len(-1L) == 2);
    check(dbr_long_len(12L) == 2);
    check(dbr_long_len(123L) == 3);
    check(dbr_long_len(1234L) == 4);
    check(dbr_long_len(12345L) == 5);
    check(dbr_long_len(-12345L) == 6);
    check(dbr_long_len(123456L) == 6);
    check(dbr_long_len(1234567L) == 7);
    check(dbr_long_len(12345678L) == 8);
    check(dbr_long_len(123456789L) == 9);
    check(dbr_long_len(1234567890L) == 10);
    check(dbr_long_len(-1234567890L) == 11);
    check(dbr_long_len(12345678901L) == 11);
    check(dbr_long_len(123456789012L) == 12);
    check(dbr_long_len(1234567890123L) == 13);
    check(dbr_long_len(12345678901234L) == 14);
    check(dbr_long_len(123456789012345L) == 15);
    check(dbr_long_len(-123456789012345L) == 16);
    check(dbr_long_len(1234567890123456L) == 16);
    check(dbr_long_len(12345678901234567L) == 17);
    check(dbr_long_len(123456789012345678L) == 18);
    check(dbr_long_len(1234567890123456789L) == 19);
    check(dbr_long_len(LONG_MIN) == 20);
    check(dbr_long_len(LONG_MAX) == 19);
}
