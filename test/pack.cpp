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

TEST_CASE(pack_posimm)
{
    char buf[1];
    const char* it = dbr_packi(buf, 63);
    check(it - buf == 1);

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == 1);

    check(i == 63);
}

TEST_CASE(pack_negimm)
{
    char buf[1];
    const char* it = dbr_packi(buf, -64);
    check(it - buf == 1);

    int i;
    it = dbr_unpacki(buf, &i);
    check(it - buf == 1);

    check(i == -64);
}
