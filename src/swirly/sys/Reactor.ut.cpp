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
#include "IoSocket.hpp"
#include "LocalAddress.hpp"
#include "Reactor.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {

struct Counters {
    int dtor{};
};

struct TestHandler : EventHandler {

    using EventHandler::EventHandler;

    explicit TestHandler(Reactor& r, Counters& cntrs) noexcept
    : EventHandler{r}
    , cntrs_{&cntrs}
    {
    }

    TestHandler() noexcept = default;
    ~TestHandler() noexcept override
    {
        if (cntrs_) {
            ++cntrs_->dtor;
        }
    }

    int matches() const { return matches_; }

  protected:
    void doClose() noexcept override {}
    void doReady(int fd, FileEvents events, Time now) override
    {
        char buf[4];
        sys::recv(fd, buf, 4, 0);
        if (strcmp(buf, "foo") == 0) {
            ++matches_;
        }
    }

  private:
    Counters* cntrs_{nullptr};
    int matches_{};
};

} // namespace

BOOST_AUTO_TEST_SUITE(ReactorSuite)

BOOST_AUTO_TEST_CASE(ReactorHandlerCase)
{
    Counters cntrs;
    Reactor r{1024};
    FileToken out, err;
    {
        BOOST_TEST(cntrs.dtor == 0);
        auto h = makeIntrusive<TestHandler>(r, cntrs);
        out = r.subscribe(STDOUT_FILENO, Reactor::Out, h);
        err = r.subscribe(STDERR_FILENO, Reactor::Out, h);
    }
    BOOST_TEST(cntrs.dtor == 0);

    r.unsubscribe(STDOUT_FILENO);
    BOOST_TEST(cntrs.dtor == 0);

    r.unsubscribe(STDERR_FILENO);
    BOOST_TEST(cntrs.dtor == 1);
}

BOOST_AUTO_TEST_CASE(ReactorFileEventsCase)
{
    using namespace literals::chrono_literals;

    Reactor r{1024};
    auto h = makeIntrusive<TestHandler>(r);

    auto socks = socketpair(LocalStream{});
    const auto tok = r.subscribe(*socks.second, Reactor::In, h);

    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches() == 0);

    socks.first.send("foo", 4, 0);
    BOOST_TEST(r.poll() == 1);
    BOOST_TEST(h->matches() == 1);

    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches() == 1);

    socks.first.send("foo\0foo", 8, 0);
    BOOST_TEST(r.poll() == 1);
    BOOST_TEST(h->matches() == 2);
    BOOST_TEST(r.poll() == 1);
    BOOST_TEST(h->matches() == 3);

    BOOST_TEST(r.poll(0ms) == 0);
    BOOST_TEST(h->matches() == 3);

    r.unsubscribe(*socks.second);
}

BOOST_AUTO_TEST_SUITE_END()
