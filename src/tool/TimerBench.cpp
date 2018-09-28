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
#include <swirly/hdr/Histogram.hpp>
#include <swirly/hdr/Recorder.hpp>

#include <swirly/sys/EpollReactor.hpp>

#include <swirly/util/Log.hpp>
#include <swirly/util/RefCount.hpp>

#include <iostream>
#include <random>

using namespace std;
using namespace swirly;

namespace {

struct TimerHandler : RefCount<TimerHandler, ThreadUnsafePolicy> {
    void on_timer(CyclTime now, Timer& tmr) {}
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

        Histogram replace_hist{1, 1'000'000, 5};
        Histogram reset_hist{1, 1'000'000, 5};

        auto h = make_intrusive<TimerHandler>();
        for (int i{0}; i < 5000000; ++i) {
            const auto now = CyclTime::set();
            auto& t = ts[dis(gen) % 128];
            if (t && dis(gen) % 2 == 0) {
                Recorder tr{reset_hist};
                t = {};
            } else {
                const auto expiry = now.wall_time() + Micros{dis(gen) % 100};
                Recorder tr{replace_hist};
                t = r.timer(expiry, Priority::High, bind<&TimerHandler::on_timer>(h.get()));
            }
            r.poll(now, 0ms);
        }

        replace_hist.percentiles_print("timer-replace.hdr", 5, 1000);
        reset_hist.percentiles_print("timer-reset.hdr", 5, 1000);
        ret = 0;

    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: " << e.what();
    }
    return ret;
}
