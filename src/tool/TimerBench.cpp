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
#include <swirly/sys/EpollReactor.hpp>

#include <swirly/util/Log.hpp>
#include <swirly/util/RefCount.hpp>

#include <iostream>
#include <random>

using namespace std;
using namespace swirly;

namespace {

struct TimerHandler : RefCount<TimerHandler, ThreadUnsafePolicy> {
    void on_timer(Timer& tmr, Time now) {}
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
        Timer ts[128];

        auto h = make_intrusive<TimerHandler>();
        for (int i{0}; i < 5000000; ++i) {
            const auto now = UnixClock::now();
            auto& t = ts[dis(gen) % 128];
            if (t && dis(gen) % 2 == 0) {
                t = {};
            } else {
                t = r.timer(now + Micros{dis(gen) % 100}, Priority::High,
                            bind<&TimerHandler::on_timer>(h.get()));
            }
            r.poll(0ms);
        }
        ret = 0;
    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: " << e.what();
    }
    return ret;
}
