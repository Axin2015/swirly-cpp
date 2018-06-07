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
#include "EpollReactor.hpp"
#include "IoSocket.hpp"
#include "LocalAddress.hpp"

#include <swirly/util/RefCount.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {

struct TestHandler : RefCount<TestHandler, ThreadUnsafePolicy> {
    void on_input(int fd, unsigned events, Time now)
    {
        char buf[4];
        os::recv(fd, buf, 4, 0);
        if (strcmp(buf, "foo") == 0) {
            ++matches;
        }
    }
    int matches{};
};

} // namespace

BOOST_AUTO_TEST_SUITE(EpollReactorSuite)

BOOST_AUTO_TEST_CASE(EpollReactorLevelCase)
{
    using namespace literals::chrono_literals;

    EpollReactor r{1024};
    auto h = make_intrusive<TestHandler>();

    auto socks = socketpair(LocalStream{});
    const auto sub = r.subscribe(*socks.second, EventIn, bind<&TestHandler::on_input>(h.get()));

    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches == 0);

    socks.first.send("foo", 4, 0);
    socks.first.send("foo", 4, 0);
    BOOST_TEST(r.poll(0ms) == 1);
    BOOST_TEST(h->matches == 1);
    BOOST_TEST(r.poll(0ms) == 1);
    BOOST_TEST(h->matches == 2);

    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches == 2);

    socks.first.send("foo", 4, 0);
    BOOST_TEST(r.poll(0ms) == 1);
    BOOST_TEST(h->matches == 3);

    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches == 3);
}

BOOST_AUTO_TEST_CASE(EpollReactorEdgeCase)
{
    using namespace literals::chrono_literals;

    EpollReactor r{1024};
    auto h = make_intrusive<TestHandler>();

    auto socks = socketpair(LocalStream{});
    auto sub = r.subscribe(*socks.second, EventIn | EventEt, bind<&TestHandler::on_input>(h.get()));

    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches == 0);

    socks.first.send("foo", 4, 0);
    socks.first.send("foo", 4, 0);
    BOOST_TEST(r.poll(0ms) == 1);
    BOOST_TEST(h->matches == 1);

    // No notification for second message.
    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches == 1);

    // Revert to level-triggered.
    sub.set_events(EventIn);
    BOOST_TEST(r.poll(0ms) == 1);
    BOOST_TEST(h->matches == 2);

    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches == 2);

    socks.first.send("foo", 4, 0);
    BOOST_TEST(r.poll(0ms) == 1);
    BOOST_TEST(h->matches == 3);

    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches == 3);
}

BOOST_AUTO_TEST_SUITE_END()
