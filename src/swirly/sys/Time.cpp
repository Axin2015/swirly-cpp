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
#include "Time.hpp"

#include <iomanip>
#include <sstream>

using namespace std;

namespace swirly {
namespace {
constexpr size_t operator""_strlen(const char* data, size_t len) noexcept
{
    return len;
}
} // namespace

SWIRLY_WEAK Time getTime() noexcept;

Time getTime() noexcept
{
    using chrono::nanoseconds;
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return Time{nanoseconds{ts.tv_sec * 1'000'000'000L + ts.tv_nsec}};
}

void printTime(Time time, ostream& os)
{
    const auto t = UnixClock::to_time_t(time);
    const auto us = usSinceEpoch(time);
    struct tm tm;
    gmtime_r(&t, &tm);

    char buf["2017-01-01 00:00:00"_strlen + 1];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    os << buf << '.' << setfill('0') << setw(6) << (us % 1'000'000L);
}

string printTime(Time time)
{
    stringstream ss;
    printTime(time, ss);
    return ss.str();
}

ostream& operator<<(ostream& os, Time time)
{
    // TODO: change to a higher resolution.
    return os << msSinceEpoch(time);
}

} // namespace swirly
