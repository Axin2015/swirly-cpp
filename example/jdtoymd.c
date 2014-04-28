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

#include <dbr/date.h>

#include <stdio.h>
#include <stdlib.h> // atoi()

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: dbr_jdtoymd <jd>\n");
        return 1;
    }

    struct DbrYmd ymd;
    dbr_jd_to_ymd(atoi(argv[1]), &ymd);
    printf("%04d%02d%02d\n", ymd.year, ymd.mon, ymd.mday);
    return 0;
}
