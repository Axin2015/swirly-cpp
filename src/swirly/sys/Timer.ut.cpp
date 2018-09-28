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
#include "Timer.hpp"

#include <boost/test/unit_test.hpp>

namespace std::chrono {
template <typename RepT, typename PeriodT>
ostream& operator<<(ostream& os, duration<RepT, PeriodT> d)
{
    return os << d.count();
}
} // namespace std::chrono

namespace swirly {
inline namespace sys {
std::ostream& operator<<(std::ostream& os, const Timer& tmr)
{
    return os << tmr.id();
}
} // namespace sys
} // namespace swirly

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(TimerSuite)

BOOST_AUTO_TEST_CASE(TimerEmptyCase)
{
    Timer t;
    BOOST_TEST(t.empty());
    BOOST_TEST(!t);
    BOOST_TEST(t.id() == 0);
    BOOST_TEST(!t.pending());
}

BOOST_AUTO_TEST_CASE(TimerInsertCase)
{
    const auto now = WallClock::now();
    TimerPool tp;
    TimerQueue tq{tp};

    auto fn = [](CyclTime now, Timer& tmr) {};
    Timer t = tq.insert(now + 2s, bind(&fn));
    BOOST_TEST(!t.empty());
    BOOST_TEST(t);
    BOOST_TEST(t.id() == 1);
    BOOST_TEST(t.pending());
    BOOST_TEST(t.expiry() == now + 2s);
    BOOST_TEST(t.interval() == 0s);
}

BOOST_AUTO_TEST_SUITE_END()
