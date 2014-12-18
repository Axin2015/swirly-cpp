/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_DATE_H
#define DBR_ASH_DATE_H

/**
 * @file
 * Julian and Gregorian date conversions.
 */

#include <assert.h>

/**
 * @addtogroup Date
 * @{
 */

/**
 * ISO8601 date in yyymmdd format.
 */

typedef int DbrIsoDate;

/**
 * Julian day.
 */

typedef int DbrJd;

/**
 * Gregorian date.
 */

struct DbrYmd {
    /**
     * Year between 1801 and 2099 inclusive.
     */
    int year;
    /**
     * Month between 1 and 12 inclusive.
     */
    int mon;
    /**
     * Day of month between 1 and 31 inclusive.
     */
    int mday;
};

/**
 * Gregorian to ISO8601 date.
 */

static inline DbrIsoDate
dbr_ymd_to_iso(int year, int mon, int mday)
{
    assert(mon <= 12);
    assert(mday <= 31);
    return year * 10000 + mon * 100 + mday;
}

/**
 * ISO8601 to Gregorian date.
 */

static inline struct DbrYmd*
dbr_iso_to_ymd(DbrIsoDate iso, struct DbrYmd* ymd)
{
    ymd->year = iso / 10000;
    ymd->mon = iso / 100 % 100;
    ymd->mday = iso % 100;
    return ymd;
}

/**
 * Gregorian date to Julian day.
 */

static inline DbrJd
dbr_ymd_to_jd(int year, int mon, int mday)
{
    // The formula given above was taken from the 1990 edition of the U.S. Naval Observatory's
    // Almanac for Computers.
    // See http://aa.usno.navy.mil/faq/docs/JD_Formula.php.

    const int i = year, j = mon, k = mday;
    return k - 32075
        + 1461 * (i + 4800 + (j - 14) / 12) / 4 + 367 * (j - 2 - (j - 14) / 12 * 12) / 12
        - 3 * ((i + 4900 + (j - 14) / 12) / 100) / 4;
}

/**
 * Julian day to Gregorian date.
 */

static inline struct DbrYmd*
dbr_jd_to_ymd(DbrJd jd, struct DbrYmd* ymd)
{
    // The formula given above was taken from the 1990 edition of the U.S. Naval Observatory's
    // Almanac for Computers.
    // See http://aa.usno.navy.mil/faq/docs/JD_Formula.php.

    int l = jd + 68569;
    const int n = 4 * l / 146097;
    l = l - (146097 * n + 3) / 4;
    int i = 4000 * (l + 1) / 1461001;
    l = l - 1461 * i / 4 + 31;
    int j = 80 * l / 2447;
    const int k = l - 2447 * j / 80;
    l = j / 11;
    j = j + 2 - 12 * l;
    i = 100 * (n - 49) + i + l;

    ymd->year = i;
    ymd->mon = j;
    ymd->mday = k;
    return ymd;
}

/**
 * ISO8601 to Julian day.
 */

static inline DbrJd
dbr_iso_to_jd(DbrIsoDate iso)
{
    struct DbrYmd ymd;
    dbr_iso_to_ymd(iso, &ymd);
    return dbr_ymd_to_jd(ymd.year, ymd.mon, ymd.mday);
}

/**
 * Julian day to ISO8601.
 */

static inline DbrIsoDate
dbr_jd_to_iso(DbrJd jd)
{
    struct DbrYmd ymd;
    dbr_jd_to_ymd(jd, &ymd);
    return dbr_ymd_to_iso(ymd.year, ymd.mon, ymd.mday);
}

/**
 * Juilian day to Modified Julian day.
 * Epoch is November 17, 1858.
 */

static inline DbrJd
dbr_jd_to_mjd(DbrJd jd)
{
    return jd - 2400000;
}

/**
 * Modified Julian day to Julian day.
 * Epoch is November 17, 1858.
 */

static inline DbrJd
dbr_mjd_to_jd(DbrJd mjd)
{
    return mjd + 2400000;
}

/**
 * Julian day to Truncated Julian day.
 * Epoch is May 24, 1968.
 */

static inline DbrJd
dbr_jd_to_tjd(DbrJd jd)
{
    return jd - 2440000;
}

/**
 * Truncated Julian day to Gregorian date.
 * Epoch is May 24, 1968.
 */

static inline DbrJd
dbr_tjd_to_jd(DbrJd tjd)
{
    return tjd + 2440000;
}

/** @} */

#endif // DBR_ASH_DATE_H
