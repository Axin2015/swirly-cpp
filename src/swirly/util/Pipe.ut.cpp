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
#include "Pipe.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {

using IntPipe = Pipe<int>;

void producer(IntPipe& p)
{
    for (int i{1}; i <= 100; ++i) {
        p.post([i](int& ref) { ref = i; });
    }
    p.close();
}

} // namespace

BOOST_AUTO_TEST_SUITE(PipeSuite)

BOOST_AUTO_TEST_CASE(PipeCase)
{
    IntPipe p{1 << 4};
    thread t{producer, ref(p)};
    int sum{0};
    while (p.fetch([&sum](int i) { sum += i; }))
        ;
    t.join();
    BOOST_TEST(sum == 5050);
}

BOOST_AUTO_TEST_SUITE_END()
