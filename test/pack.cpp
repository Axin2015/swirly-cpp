/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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

TEST_CASE(pack16)
{
    char buf[sizeof(uint16_t)];
    const char* it = dbr_pack_uint16(buf, (uint16_t)101);
    check(it == buf + sizeof(buf));

    uint16_t i;
    it = dbr_unpack_uint16(buf, &i);
    check(it == buf + sizeof(buf));

    check(i == 101);
}

TEST_CASE(pack32)
{
    char buf[sizeof(uint32_t)];
    const char* it = dbr_pack_uint32(buf, (uint32_t)101);
    check(it == buf + sizeof(buf));

    uint32_t i;
    it = dbr_unpack_uint32(buf, &i);
    check(it == buf + sizeof(buf));

    check(i == 101);
}

TEST_CASE(pack64)
{
    char buf[sizeof(uint64_t)];
    const char* it = dbr_pack_uint64(buf, (uint64_t)101);
    check(it == buf + sizeof(buf));

    uint64_t i;
    it = dbr_unpack_uint64(buf, &i);
    check(it == buf + sizeof(buf));

    check(i == 101);
}

TEST_CASE(packf_II)
{
    char buf[sizeof(uint32_t) + sizeof(uint32_t)];
    const char* it = dbr_packf(buf, "II", (uint32_t)101, (uint32_t)202);
    check(it == buf + sizeof(buf));

    uint32_t i, j;
    it = dbr_unpackf(buf, "II", &i, &j);
    check(it == buf + sizeof(buf));

    check(i == 101);
    check(j == 202);
}
