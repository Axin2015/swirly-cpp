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

#include <dbr/ash/date.h>

#include <stdio.h>
#include <stdlib.h> // atoi()

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: dbr_ymdtojd <ymd>\n");
        return 1;
    }
    int ymd = atoi(argv[1]);
    const int day = ymd % 100;
    ymd /= 100;
    const int mon = ymd % 100;
    ymd /= 100;
    const int year = ymd;
    const DbrJd jd = dbr_ymd_to_jd(year, mon, day);
    printf("%d\n", jd);
    return 0;
}
