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
#include <swirly/util/Log.hpp>

#include <swirly/sys/EpollReactor.hpp>

#include <iostream>
#include <queue>
#include <random>

using namespace std;
using namespace swirly;

namespace {

struct TimerHandler : EventHandler {

    using EventHandler::EventHandler;

    ~TimerHandler() noexcept override = default;

    void doClose() noexcept override {}
    void doTimer(Timer& tmr, Time now) override {}
};

} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {

        random_device rd;
        mt19937 gen{rd()};
        uniform_int_distribution<> dis;

        EpollReactor r{1024};
        priority_queue<pair<int, Timer>> pq;

        auto h = makeIntrusive<TimerHandler>(r);
        for (int i{0}; i < 1000000; ++i) {
            const auto now = UnixClock::now();
            if (pq.empty() || dis(gen) % 2 == 0) {
                pq.emplace(i, r.timer(now + Micros{dis(gen) % 100}, Priority::High, h));
            } else {
                auto tmr = pq.top().second;
                pq.pop();
                tmr.reset();
            }
            if (!pq.empty()) {
                r.poll(0ms);
            }
        }
        ret = 0;
    } catch (const exception& e) {
        SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
}
