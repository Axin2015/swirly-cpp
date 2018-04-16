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
#include "Worker.hpp"

#include "Journ.hpp"
#include "MsgQueue.hpp"

#include <swirly/sys/Cpu.hpp>
#include <swirly/sys/Signal.hpp>

#include <swirly/util/Log.hpp>

namespace swirly {
inline namespace fin {
using namespace std;
namespace {
void runJourn(MsgQueue& mq, Journ& journ)
{
    sigBlockAll();
    SWIRLY_NOTICE << "started journal thread"sv;
    try {
        CpuBackoff backoff;
        for (;;) {
            Msg msg;
            while (mq.pop(msg)) {
                journ.write(msg);
                backoff.reset();
            }
            if (journ.interrupted()) {
                break;
            }
            backoff();
        }
    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: "sv << e.what();
        kill(0, SIGTERM);
    }
    SWIRLY_NOTICE << "stopping journal thread"sv;
}
} // namespace

JournWorker::JournWorker(MsgQueue& mq, Journ& journ)
: mq_(mq)
, thread_{runJourn, ref(mq), ref(journ)}
{
}

JournWorker::~JournWorker()
{
    if (!mq_.interrupt(1_id32)) {
        SWIRLY_ERROR << "interrupt timeout"sv;
    }
    thread_.join();
}

} // namespace fin
} // namespace swirly
