/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */

#include <dbr/ash/date.h>

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
