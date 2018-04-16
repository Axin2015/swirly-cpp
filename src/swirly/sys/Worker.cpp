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

#include "Reactor.hpp"
#include "Signal.hpp"

#include <swirly/util/Log.hpp>

namespace swirly {
inline namespace sys {
using namespace std;
namespace {
void runReactor(Reactor& r)
{
    sigBlockAll();
    SWIRLY_NOTICE << "started reactor thread"sv;
    try {
        while (!r.interrupted()) {
            r.poll();
        }
    } catch (const exception& e) {
        SWIRLY_ERROR << "exception: "sv << e.what();
        kill(0, SIGTERM);
    }
    SWIRLY_NOTICE << "stopping reactor thread"sv;
}
} // namespace

ReactorWorker::ReactorWorker(Reactor& r)
: reactor_(r)
, thread_{runReactor, ref(r)}
{
}

ReactorWorker::~ReactorWorker()
{
    reactor_.interrupt(1);
    thread_.join();
}

} // namespace sys
} // namespace swirly
