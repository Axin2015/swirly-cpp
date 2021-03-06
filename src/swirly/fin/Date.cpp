/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#include "Date.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#pragma GCC diagnostic pop

namespace lt = boost::local_time;
namespace pt = boost::posix_time;

namespace swirly {
inline namespace fin {
using namespace std;

BusinessDay::BusinessDay(const char* time_zone)
: time_zone_{new lt::posix_time_zone{time_zone}}
{
}

BusinessDay::~BusinessDay() = default;

// Copy.
BusinessDay::BusinessDay(const BusinessDay&) noexcept = default;
BusinessDay& BusinessDay::operator=(const BusinessDay&) = default;

// Move.
BusinessDay::BusinessDay(BusinessDay&&) noexcept = default;
BusinessDay& BusinessDay::operator=(BusinessDay&&) noexcept = default;

JDay BusinessDay::operator()(WallTime time) const
{
    const auto t = WallClock::to_time_t(time);

    // Returned cached value if it exists.
    const auto i = t & 1;
    if (cache_[i].first == t) {
        return cache_[i].second;
    }
    lt::local_date_time ldt{pt::from_time_t(t), time_zone_};
    const auto jd = JDay{ldt.local_time().date().julian_day()};
    // Update cache entry.
    cache_[i] = {t, jd};
    return jd;
}

} // namespace fin
} // namespace swirly
