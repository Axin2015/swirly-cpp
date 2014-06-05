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

#include <dbr/pack.h>

#include <limits.h>

TEST_CASE(pack_negimm)
{
    char buf[1];
    const char* it = dbr_packi(buf, -64);
    check(it - buf == sizeof(buf));

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == -64);
}

TEST_CASE(pack_posimm)
{
    char buf[1];
    const char* it = dbr_packi(buf, 63);
    check(it - buf == sizeof(buf));

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == 63);
}

TEST_CASE(pack_negi8)
{
    char buf[2];
    const char* it = dbr_packi(buf, SCHAR_MIN);
    check(it - buf == sizeof(buf));

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == SCHAR_MIN);
}

TEST_CASE(pack_posi8)
{
    char buf[2];
    const char* it = dbr_packi(buf, SCHAR_MAX);
    check(it - buf == sizeof(buf));

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == SCHAR_MAX);
}

TEST_CASE(pack_negi16)
{
    char buf[3];
    const char* it = dbr_packi(buf, SHRT_MIN);
    check(it - buf == sizeof(buf));

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == SHRT_MIN);
}

TEST_CASE(pack_posi16)
{
    char buf[3];
    const char* it = dbr_packi(buf, SHRT_MAX);
    check(it - buf == sizeof(buf));

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == SHRT_MAX);
}

TEST_CASE(pack_negi32)
{
    char buf[5];
    const char* it = dbr_packi(buf, INT_MIN);
    check(it - buf == sizeof(buf));

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == INT_MIN);
}

TEST_CASE(pack_posi32)
{
    char buf[5];
    const char* it = dbr_packi(buf, INT_MAX);
    check(it - buf == sizeof(buf));

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == INT_MAX);
}

TEST_CASE(pack_negi64)
{
    char buf[9];
    const char* it = dbr_packl(buf, LONG_MIN);
    check(it - buf == sizeof(buf));

    long l;
    it = dbr_unpackl(buf, &l);
    check(it - buf == sizeof(buf));

    check(l == LONG_MIN);
}

TEST_CASE(pack_posi64)
{
    char buf[9];
    const char* it = dbr_packl(buf, LONG_MAX);
    check(it - buf == sizeof(buf));

    long l;
    it = dbr_unpackl(buf, &l);
    check(it - buf == sizeof(buf));

    check(l == LONG_MAX);
}

TEST_CASE(pack_posf)
{
    char buf[1 + 2 + 3 + 5 + 9];
    const char* it = dbr_packf(buf, "iiiil", 63, SCHAR_MAX, SHRT_MAX, INT_MAX, LONG_MAX);
    check(it - buf == sizeof(buf));

    int i[4];
    long l;
    it = dbr_unpackf(buf, "iiiil", &i[0], &i[1], &i[2], &i[3], &l);
    check(it - buf == sizeof(buf));

    check(i[0] == 63);
    check(i[1] == SCHAR_MAX);
    check(i[2] == SHRT_MAX);
    check(i[3] == INT_MAX);
    check(l == LONG_MAX);
}

TEST_CASE(pack_negf)
{
    char buf[1 + 2 + 3 + 5 + 9];
    const char* it = dbr_packf(buf, "iiiil", -64, SCHAR_MIN, SHRT_MIN, INT_MIN, LONG_MIN);
    check(it - buf == sizeof(buf));

    int i[4];
    long l;
    it = dbr_unpackf(buf, "iiiil", &i[0], &i[1], &i[2], &i[3], &l);
    check(it - buf == sizeof(buf));

    check(i[0] == -64);
    check(i[1] == SCHAR_MIN);
    check(i[2] == SHRT_MIN);
    check(i[3] == INT_MIN);
    check(l == LONG_MIN);
}

TEST_CASE(pack_strf)
{
    char buf[1 + 4];
    const char* it = dbr_packf(buf, "m", "test");
    check(it - buf == sizeof(buf));

    char s[5];
    it = dbr_unpackf(buf, "m", s);
    check(it - buf == sizeof(buf));

    check(strcmp(s, "test") == 0);
}

TEST_CASE(pack_leni)
{
    check(dbr_pack_leni(-64) == 1);
    check(dbr_pack_leni(63) == 1);
    check(dbr_pack_leni(SCHAR_MIN) == 2);
    check(dbr_pack_leni(SCHAR_MAX) == 2);
    check(dbr_pack_leni(SHRT_MIN) == 3);
    check(dbr_pack_leni(SHRT_MAX) == 3);
    check(dbr_pack_leni(INT_MIN) == 5);
    check(dbr_pack_leni(INT_MAX) == 5);
}

TEST_CASE(pack_lenl)
{
    check(dbr_pack_lenl(LONG_MIN) == 9);
    check(dbr_pack_lenl(LONG_MAX) == 9);
}

TEST_CASE(pack_lens)
{
    check(dbr_pack_lens("test", 16) == 5);
    check(dbr_pack_lens("test", 3) == 4);
}

TEST_CASE(pack_lenf)
{
    check(dbr_pack_lenf("iiiilm", 63, SCHAR_MAX, SHRT_MAX, INT_MAX, LONG_MAX, "test") == 25);
}
