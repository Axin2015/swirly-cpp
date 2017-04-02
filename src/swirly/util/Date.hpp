/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLY_UTIL_DATE_HPP
#define SWIRLY_UTIL_DATE_HPP

#include <swirly/util/IntWrapper.hpp>
#include <swirly/util/Time.hpp>

#include <cassert>

namespace swirly {

struct IsoDatePolicy : Int32Policy {
};
struct JDayPolicy : Int32Policy {
};

/**
 * ISO8601 date in yyymmdd format.
 */
using IsoDate = IntWrapper<IsoDatePolicy>;

constexpr auto operator""_ymd(unsigned long long val) noexcept
{
    return IsoDate{val};
}

/**
 * Julian day.
 */
using JDay = IntWrapper<JDayPolicy>;

constexpr auto operator""_jd(unsigned long long val) noexcept
{
    return JDay{val};
}

/**
 * Gregorian to ISO8601 date.
 */
inline auto ymdToIso(int year, int mon, int mday) noexcept
{
    assert(mon <= 11);
    assert(mday <= 31);
    return IsoDate{year * 10000 + (mon + 1) * 100 + mday};
}

/**
 * Gregorian date to Julian day.
 */
constexpr auto ymdToJd(int year, int mon, int mday) noexcept
{
    // The formula given below was taken from the 1990 edition of the U.S. Naval Observatory's Almanac
    // for Computers.
    // See http://aa.usno.navy.mil/faq/docs/JD_Formula.php.

    const auto i = year;
    const auto j = mon + 1;
    const auto k = mday;
    return JDay{k - 32075 + 1461 * (i + 4800 + (j - 14) / 12) / 4
                + 367 * (j - 2 - (j - 14) / 12 * 12) / 12
                - 3 * ((i + 4900 + (j - 14) / 12) / 100) / 4};
}

/**
 * ISO8601 to Julian day.
 */
constexpr auto isoToJd(IsoDate iso) noexcept
{
    const auto n = iso.count();
    const auto year = n / 10000;
    const auto mon = (n / 100 % 100) - 1;
    const auto mday = n % 100;
    return ymdToJd(year, mon, mday);
}

/**
 * Julian day to ISO8601.
 */
constexpr auto jdToIso(JDay jd) noexcept
{
    // The formula given above was taken from the 1990 edition of the U.S. Naval Observatory's
    // Almanac for Computers.
    // See http://aa.usno.navy.mil/faq/docs/JD_Formula.php.

    auto l = jd.count() + 68569;
    const auto n = 4 * l / 146097;
    l = l - (146097 * n + 3) / 4;
    auto i = 4000 * (l + 1) / 1461001;
    l = l - 1461 * i / 4 + 31;
    auto j = 80 * l / 2447;
    const auto k = l - 2447 * j / 80;
    l = j / 11;
    j = j + 2 - 12 * l;
    i = 100 * (n - 49) + i + l;

    return IsoDate{i * 10000 + j * 100 + k};
}

/**
 * Juilian day to Modified Julian day. Epoch is November 17, 1858.
 */
constexpr std::int32_t jdToMjd(JDay jd) noexcept
{
    return jd.count() - 2400000;
}

/**
 * Modified Julian day to Julian day. Epoch is November 17, 1858.
 */
constexpr auto mjdToJd(std::int32_t mjd) noexcept
{
    return JDay{mjd + 2400000};
}

/**
 * Julian day to Truncated Julian day. Epoch is May 24, 1968.
 */
constexpr std::int32_t jdToTjd(JDay jd) noexcept
{
    return jd.count() - 2440000;
}

/**
 * Truncated Julian day to Gregorian date. Epoch is May 24, 1968.
 */
constexpr JDay tjdToJd(std::int32_t tjd) noexcept
{
    return JDay{tjd + 2440000};
}

/**
 * Julian day to Unix time.
 */
constexpr Time jdToTime(JDay jd) noexcept
{
    // Julian day for January 1st, 1970.
    const JDay jdUnixEpoc = 2440588_jd;
    const std::int64_t msInDay = 24 * 60 * 60 * 1000;
    // Add half day for 12pm.
    return msToTime((jd - jdUnixEpoc).count() * msInDay + (msInDay >> 1));
}

/**
 * Julian day to ISO8601 if argument is non-zero.
 */
constexpr IsoDate maybeJdToIso(JDay jd) noexcept
{
    return jd != 0_jd ? jdToIso(jd) : 0_ymd;
}

/**
 * ISO8601 to Julian day if argument is non-zero.
 */
constexpr JDay maybeIsoToJd(IsoDate iso) noexcept
{
    return iso != 0_ymd ? isoToJd(iso) : 0_jd;
}

} // swirly

#endif // SWIRLY_UTIL_DATE_HPP
