/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <sc/ash/pack.h>

#include <limits.h>

TEST_CASE(pack_negimm)
{
    char buf[1];
    const char* it = sc_packi(buf, -64);
    check(it - buf == sizeof(buf));

    int i;
    it = sc_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == -64);
}

TEST_CASE(pack_posimm)
{
    char buf[1];
    const char* it = sc_packi(buf, 63);
    check(it - buf == sizeof(buf));

    int i;
    it = sc_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == 63);
}

TEST_CASE(pack_negi8)
{
    char buf[2];
    const char* it = sc_packi(buf, SCHAR_MIN);
    check(it - buf == sizeof(buf));

    int i;
    it = sc_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == SCHAR_MIN);
}

TEST_CASE(pack_posi8)
{
    char buf[2];
    const char* it = sc_packi(buf, SCHAR_MAX);
    check(it - buf == sizeof(buf));

    int i;
    it = sc_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == SCHAR_MAX);
}

TEST_CASE(pack_negi16)
{
    char buf[3];
    const char* it = sc_packi(buf, SHRT_MIN);
    check(it - buf == sizeof(buf));

    int i;
    it = sc_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == SHRT_MIN);
}

TEST_CASE(pack_posi16)
{
    char buf[3];
    const char* it = sc_packi(buf, SHRT_MAX);
    check(it - buf == sizeof(buf));

    int i;
    it = sc_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == SHRT_MAX);
}

TEST_CASE(pack_negi32)
{
    char buf[5];
    const char* it = sc_packi(buf, INT_MIN);
    check(it - buf == sizeof(buf));

    int i;
    it = sc_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == INT_MIN);
}

TEST_CASE(pack_posi32)
{
    char buf[5];
    const char* it = sc_packi(buf, INT_MAX);
    check(it - buf == sizeof(buf));

    int i;
    it = sc_unpacki(buf, &i);
    check(it - buf == sizeof(buf));

    check(i == INT_MAX);
}

TEST_CASE(pack_negi64)
{
    char buf[9];
    const char* it = sc_packl(buf, LONG_MIN);
    check(it - buf == sizeof(buf));

    long l;
    it = sc_unpackl(buf, &l);
    check(it - buf == sizeof(buf));

    check(l == LONG_MIN);
}

TEST_CASE(pack_posi64)
{
    char buf[9];
    const char* it = sc_packl(buf, LONG_MAX);
    check(it - buf == sizeof(buf));

    long l;
    it = sc_unpackl(buf, &l);
    check(it - buf == sizeof(buf));

    check(l == LONG_MAX);
}

TEST_CASE(pack_posf)
{
    char buf[1 + 2 + 3 + 5 + 9];
    const char* it = sc_packf(buf, "iiiil", 63, SCHAR_MAX, SHRT_MAX, INT_MAX, LONG_MAX);
    check(it - buf == sizeof(buf));

    int i[4];
    long l;
    it = sc_unpackf(buf, "iiiil", &i[0], &i[1], &i[2], &i[3], &l);
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
    const char* it = sc_packf(buf, "iiiil", -64, SCHAR_MIN, SHRT_MIN, INT_MIN, LONG_MIN);
    check(it - buf == sizeof(buf));

    int i[4];
    long l;
    it = sc_unpackf(buf, "iiiil", &i[0], &i[1], &i[2], &i[3], &l);
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
    const char* it = sc_packf(buf, "m", "test");
    check(it - buf == sizeof(buf));

    char s[5];
    it = sc_unpackf(buf, "m", s);
    check(it - buf == sizeof(buf));

    check(strcmp(s, "test") == 0);
}

TEST_CASE(pack_leni)
{
    check(sc_pack_leni(-64) == 1);
    check(sc_pack_leni(63) == 1);
    check(sc_pack_leni(SCHAR_MIN) == 2);
    check(sc_pack_leni(SCHAR_MAX) == 2);
    check(sc_pack_leni(SHRT_MIN) == 3);
    check(sc_pack_leni(SHRT_MAX) == 3);
    check(sc_pack_leni(INT_MIN) == 5);
    check(sc_pack_leni(INT_MAX) == 5);
}

TEST_CASE(pack_lenl)
{
    check(sc_pack_lenl(LONG_MIN) == 9);
    check(sc_pack_lenl(LONG_MAX) == 9);
}

TEST_CASE(pack_lens)
{
    check(sc_pack_lens("test", 16) == 5);
    check(sc_pack_lens("test", 3) == 4);
}

TEST_CASE(pack_lenf)
{
    check(sc_pack_lenf("iiiilm", 63, SCHAR_MAX, SHRT_MAX, INT_MAX, LONG_MAX, "test") == 25);
}
