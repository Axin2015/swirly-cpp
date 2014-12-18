/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */

#include <sc/ash/date.h>

#include <stdio.h>
#include <stdlib.h> // atoi()

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: sc_jdtoymd <jd>\n");
        return 1;
    }

    struct ScYmd ymd;
    sc_jd_to_ymd(atoi(argv[1]), &ymd);
    printf("%04d%02d%02d\n", ymd.year, ymd.mon, ymd.mday);
    return 0;
}
