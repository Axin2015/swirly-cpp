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
#include <swirly/util/Profile.hpp>

#include <swirly/sys/BoundedQueue.hpp>

#include <iostream>
#include <thread>

#include <xmmintrin.h>

using namespace std;
using namespace swirly;

int main(int argc, char* argv[])
{
    using Clock = chrono::high_resolution_clock;

    int ret = 1;
    try {
        Profile p{"BoundedQueue"sv};
        BoundedQueue<Clock::time_point> q{1 << 14};

        enum { Iters = 50000000 };
        auto t1 = thread([&q]() {
            for (int i{}; i < Iters;) {
                if (q.push(Clock::now())) {
                    ++i;
                } else {
                    sched_yield();
                }
            }
        });

        auto t2 = thread([&q]() {
            for (int i{}; i < Iters;) {
                if (q.push(Clock::now())) {
                    ++i;
                } else {
                    sched_yield();
                }
            }
        });

        for (int i{}; i < Iters * 2;) {

            Clock::time_point start;
            if (q.pop(start)) {
                if (++i <= 1000) {
                    // Warmup.
                    continue;
                }
                const auto end = Clock::now();
                const chrono::duration<double, micro> diff{end - start};
                const auto usec = diff.count();
                p.record(usec);
            } else {
                _mm_pause();
            }
        }

        t1.join();
        t2.join();
        p.report();
        ret = 0;

    } catch (const exception& e) {
        SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
    return ret;
}
