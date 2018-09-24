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
#include <swirly/hdr/File.hpp>
#include <swirly/hdr/Histogram.hpp>

#include <swirly/app/Backoff.hpp>
#include <swirly/app/MemQueue.hpp>

#include <swirly/util/Log.hpp>

#include <iostream>
#include <thread>

using namespace std;
using namespace swirly;

int main(int argc, char* argv[])
{
    using Clock = chrono::high_resolution_clock;

    int ret = 1;
    try {
        Histogram hist{1, 1'000'000, 5};
        MemQueue<Clock::duration> q{1 << 14};

        enum { Iters = 50000000 };
        auto t1 = thread([&q]() {
            for (int i{}; i < Iters;) {
                if (q.push(Clock::now().time_since_epoch())) {
                    ++i;
                } else {
                    sched_yield();
                }
            }
        });

        auto t2 = thread([&q]() {
            for (int i{}; i < Iters;) {
                if (q.push(Clock::now().time_since_epoch())) {
                    ++i;
                } else {
                    sched_yield();
                }
            }
        });

        for (int i{}; i < Iters * 2;) {

            Clock::duration start;
            if (q.pop(start)) {
                if (++i <= 1000) {
                    // Warmup.
                    continue;
                }
                const auto end = Clock::now().time_since_epoch();
                const chrono::duration<double, micro> diff{end - start};
                const auto usec = diff.count();
                hist.record_value(usec);
            } else {
                cpu_relax();
            }
        }

        t1.join();
        t2.join();

        auto file = open_file("queue.hdr", "w");
        hist.percentiles_print(file.get(), 5, 1000);
        ret = 0;

    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: " << e.what();
    }
    return ret;
}
