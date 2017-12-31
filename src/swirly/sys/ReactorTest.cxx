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
#include "IoSocket.hpp"
#include "LocalAddress.hpp"
#include "Reactor.hpp"

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

namespace {

struct Counters {
    int dtor{};
};

struct TestActor : Actor {

    using Actor::Actor;

    explicit TestActor(Reactor& reactor, Counters& cntrs) noexcept
      : Actor{reactor}
      , cntrs_{&cntrs}
    {
    }

    TestActor() noexcept = default;
    ~TestActor() noexcept override
    {
        if (cntrs_) {
            ++cntrs_->dtor;
        }
    }

    int matches() const { return matches_; }

  protected:
    void doEvent(const Event& event) override {}
    void doReady(int fd, FileEvents events, Time now) override
    {
        char buf[4];
        sys::recv(fd, buf, 4, 0);
        if (strcmp(buf, "foo") == 0) {
            ++matches_;
        }
    }
    void doSignal(int sig) override {}
    void doTimer(const Timer& tmr, Time now) override {}

  private:
    Counters* cntrs_{nullptr};
    int matches_{};
};

} // namespace

SWIRLY_TEST_CASE(ReactorHandler)
{
    Counters cntrs;
    Reactor r{1024};
    FileToken out, err;
    {
        SWIRLY_CHECK(cntrs.dtor == 0);
        auto a = makeIntrusive<TestActor>(r, cntrs);
        out = r.subscribe(STDOUT_FILENO, Reactor::Out, a);
        err = r.subscribe(STDERR_FILENO, Reactor::Out, a);
    }
    SWIRLY_CHECK(cntrs.dtor == 0);

    r.unsubscribe(STDOUT_FILENO);
    SWIRLY_CHECK(cntrs.dtor == 0);

    r.unsubscribe(STDERR_FILENO);
    SWIRLY_CHECK(cntrs.dtor == 1);
}

SWIRLY_TEST_CASE(ReactorFileEvents)
{
    using namespace literals::chrono_literals;

    Reactor r{1024};
    auto a = makeIntrusive<TestActor>(r);

    auto socks = socketpair(LocalStream{});
    const auto tok = r.subscribe(*socks.second, Reactor::In, a);

    SWIRLY_CHECK(r.poll(0ms) == 0);
    SWIRLY_CHECK(a->matches() == 0);

    socks.first.send("foo", 4, 0);
    SWIRLY_CHECK(r.poll() == 1);
    SWIRLY_CHECK(a->matches() == 1);

    SWIRLY_CHECK(r.poll(0ms) == 0);
    SWIRLY_CHECK(a->matches() == 1);

    socks.first.send("foo\0foo", 8, 0);
    SWIRLY_CHECK(r.poll() == 1);
    SWIRLY_CHECK(a->matches() == 2);
    SWIRLY_CHECK(r.poll() == 1);
    SWIRLY_CHECK(a->matches() == 3);

    SWIRLY_CHECK(r.poll(0ms) == 0);
    SWIRLY_CHECK(a->matches() == 3);

    r.unsubscribe(*socks.second);
}
