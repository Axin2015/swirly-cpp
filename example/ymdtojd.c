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
        fprintf(stderr, "usage: sc_ymdtojd <ymd>\n");
        return 1;
    }
    int ymd = atoi(argv[1]);
    const int day = ymd % 100;
    ymd /= 100;
    const int mon = ymd % 100;
    ymd /= 100;
    const int year = ymd;
    const ScJd jd = sc_ymd_to_jd(year, mon, day);
    printf("%d\n", jd);
    return 0;
}
