/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef SWIRLY_ASH_JULIANDAY_HPP
#define SWIRLY_ASH_JULIANDAY_HPP

#include <swirly/ash/Types.hpp>

#include <cassert>

namespace swirly {

/**
 * @addtogroup Date
 * @{
 */

/**
 * Gregorian to ISO8601 date.
 */
inline constexpr IsoDate ymdToIso(int year, int mon, int mday)
{
    assert(mon <= 11);
    assert(mday <= 31);
    return year * 10000 + (mon + 1) * 100 + mday;
}

/**
 * Gregorian date to Julian day.
 */
inline constexpr Jd ymdToJd(int year, int mon, int mday)
{
    // The formula given below was taken from the 1990 edition of the U.S. Naval Observatory's
    // Almanac for Computers.
    // See http://aa.usno.navy.mil/faq/docs/JD_Formula.php.

    const auto i = year;
    const auto j = mon + 1;
    const auto k = mday;
    return k - 32075 + 1461 * (i + 4800 + (j - 14) / 12) / 4
        + 367 * (j - 2 - (j - 14) / 12 * 12) / 12
        - 3 * ((i + 4900 + (j - 14) / 12) / 100) / 4;
}

/**
 * ISO8601 to Julian day.
 */
inline constexpr Jd isoToJd(IsoDate iso)
{
    const auto year = iso / 10000;
    const auto mon = (iso / 100 % 100) - 1;
    const auto mday = iso % 100;
    return ymdToJd(year, mon, mday);
}

/**
 * Julian day to ISO8601.
 */
inline constexpr IsoDate jdToIso(Jd jd)
{
    // The formula given above was taken from the 1990 edition of the U.S. Naval Observatory's
    // Almanac for Computers.
    // See http://aa.usno.navy.mil/faq/docs/JD_Formula.php.

    auto l = jd + 68569;
    const auto n = 4 * l / 146097;
    l = l - (146097 * n + 3) / 4;
    auto i = 4000 * (l + 1) / 1461001;
    l = l - 1461 * i / 4 + 31;
    auto j = 80 * l / 2447;
    const auto k = l - 2447 * j / 80;
    l = j / 11;
    j = j + 2 - 12 * l;
    i = 100 * (n - 49) + i + l;

    return i * 10000 + j * 100 + k;
}

/**
 * Juilian day to Modified Julian day. Epoch is November 17, 1858.
 */
inline constexpr Jd jdToMjd(Jd jd)
{
    return jd - 2400000;
}

/**
 * Modified Julian day to Julian day. Epoch is November 17, 1858.
 */
inline constexpr Jd mjdToJd(Jd mjd)
{
    return mjd + 2400000;
}

/**
 * Julian day to Truncated Julian day. Epoch is May 24, 1968.
 */
inline constexpr Jd jdToTjd(Jd jd)
{
    return jd - 2440000;
}

/**
 * Truncated Julian day to Gregorian date. Epoch is May 24, 1968.
 */
inline constexpr Jd tjdToJd(Jd tjd)
{
    return tjd + 2440000;
}

/**
 * Julian day to milliseconds since Unix epoch.
 */
inline constexpr Millis jdToMillis(Jd jd)
{
    // Julian day for January 1st, 1970.
    const Jd jdUnixEpoc = 2440588;
    const Millis millisInDay = 24 * 60 * 60 * 1000;
    // Add half day for 12pm.
    return (jd - jdUnixEpoc) * millisInDay + (millisInDay >> 1);
}

/**
 * Julian day to ISO8601 if argument is non-zero.
 */
inline constexpr IsoDate maybeJdToIso(Jd jd)
{
    return jd != 0 ? jdToIso(jd) : 0;
}

/**
 * ISO8601 to Julian day if argument is non-zero.
 */
inline constexpr Jd maybeIsoToJd(IsoDate iso)
{
    return iso != 0 ? isoToJd(iso) : 0;
}

/** @} */

} // swirly

#endif // SWIRLY_ASH_JULIANDAY_HPP
