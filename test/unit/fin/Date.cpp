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
#include <swirly/fin/Date.hpp>

#include <swirly/util/Date.hpp>

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(GetBusDay)
{
    BusinessDay busDay{RollHour, NewYork};

    // Business days roll at 5pm New York.

    // Friday, March 14, 2014
    // 21.00 UTC
    // 17.00 EDT (UTC-4 hours)

    // 20.59 UTC
    SWIRLY_CHECK(busDay(Time{1394830799000ms}) == ymdToJd(2014, 2, 14));

    // 21.00 UTC
    SWIRLY_CHECK(busDay(Time{1394830800000ms}) == ymdToJd(2014, 2, 15));
}
