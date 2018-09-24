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
#include <swirly/hdr/IntervalRecorder.hpp>
#include <swirly/hdr/LogWriter.hpp>

#include <swirly/sys/EpollReactor.hpp>
#include <swirly/sys/Signal.hpp>

#include <atomic>
#include <iostream>
#include <thread>

using namespace std;
using namespace swirly;

namespace {
class HiccupThread {
  public:
    explicit HiccupThread(IntervalRecorder& recorder)
    : thread_{run, ref(recorder), cref(stop_)}
    {
    }
    ~HiccupThread()
    {
        stop_.store(true, memory_order_relaxed);
        thread_.join();
    }

  private:
    static void run(IntervalRecorder& recorder, const atomic<bool>& stop)
    {
        sig_block_all();
        try {
            EpollReactor r{1};
            while (!stop.load(memory_order_relaxed)) {
                const auto start = WallClock::now();
                auto fn = [&recorder, start](WallTime now, Timer& tmr) {
                    const chrono::duration<double, micro> diff{now - start};
                    const auto usec = diff.count();
                    recorder.record_value(usec);
                };
                auto t = r.timer(start + 1ms, Priority::Low, bind(&fn));
                while (!r.poll())
                    ;
            }
        } catch (const std::exception& e) {
            cerr << "exception: " << e.what() << endl;
            kill(getpid(), SIGTERM);
        }
        cout << "exiting\n";
    }
    atomic<bool> stop_{false};
    thread thread_;
};
} // namespace

int main(int argc, char* argv[])
{
    int ret = 1;
    try {
        auto start_time = WallClock::now();

        IntervalRecorder recorder{1, 1'000'000, 5};
        LogWriter writer{stdout};
        writer.write_header(start_time, "hiccup");

        hdr_histogram* inactive{nullptr};

        HiccupThread thread{recorder};
        SigWait sig_wait;
        for (;;) {
            switch (const auto sig = sig_wait(1s)) {
            case 0: {
                inactive = recorder.sample_and_recycle(inactive);
                assert(inactive);

                const auto end_time = WallClock::now();
                writer.write(start_time, end_time, *inactive);
                writer.flush();
                clear(*inactive);
                start_time = end_time;
                continue;
            }
            case SIGHUP:
                cout << "received SIGHUP\n";
                continue;
            case SIGINT:
                cout << "received SIGINT\n";
                break;
            case SIGTERM:
                cout << "received SIGTERM\n";
                break;
            default:
                cout << "received signal: " << sig << endl;
                continue;
            }
            break;
        }
        ret = 0;
    } catch (const exception& e) {
        cerr << "exception: " << e.what();
    }
    return ret;
}
