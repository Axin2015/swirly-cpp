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
#ifndef DBR_DATE_H
#define DBR_DATE_H

/**
 * @addtogroup Date
 * @{
 */

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

static inline void
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
}

/**
 * Gregorian date to Modified Julian day.
 * Epoch is November 17, 1858.
 */

static inline DbrJd
dbr_ymd_to_mjd(int year, int mon, int mday)
{
    return dbr_ymd_to_jd(year, mon, mday) - 2400000;
}

/**
 * Modified Julian day to Gregorian date.
 * Epoch is November 17, 1858.
 */

static inline void
dbr_mjd_to_ymd(DbrJd mjd, struct DbrYmd* ymd)
{
    return dbr_mjd_to_ymd(mjd + 2400000, ymd);
}

/**
 * Gregorian date to Truncated Julian day.
 * Epoch is May 24, 1968.
 */

static inline DbrJd
dbr_ymd_to_tjd(int year, int mon, int mday)
{
    return dbr_ymd_to_jd(year, mon, mday) - 2440000;
}

/**
 * Truncated Julian day to Gregorian date.
 * Epoch is May 24, 1968.
 */

static inline void
dbr_tjd_to_ymd(DbrJd tjd, struct DbrYmd* ymd)
{
    return dbr_mjd_to_ymd(tjd + 2440000, ymd);
}

/** @} */

#endif // DBR_DATE_H
