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
#include "Initiator.hpp"

#include <swirly/util/Log.hpp>

namespace swirly {
inline namespace fix {
using namespace std;

FixInitiator::FixInitiator(Time now, Reactor& r, const Endpoint& ep, const FixConfig& config,
                           const FixSessId& sess_id, FixApp& app)
: reactor_(r)
, ep_{ep}
, config_(config)
, sess_id_{sess_id}
, app_(app)
{
    // Immediate and then at 2s intervals.
    tmr_ = reactor_.timer(now, 2s, Priority::Low, bind<&FixInitiator::on_timer>(this));
}

FixInitiator::~FixInitiator()
{
    conn_list_.clear_and_dispose([](auto* conn) { delete conn; });
}

void FixInitiator::do_connect(Time now, IoSocket&& sock, const Endpoint& ep)
{
    inprogress_ = false;

    // High performance TCP servers could use a custom allocator.
    auto* const conn = new FixConn{now, reactor_, move(sock), ep, config_, app_};
    conn_list_.push_back(*conn);
    conn->logon(now, sess_id_);
}

void FixInitiator::do_connect_error(Time now, const std::exception& e)
{
    SWIRLY_ERROR << "failed to connect: " << e.what();
    inprogress_ = false;
}

void FixInitiator::on_timer(Time now, Timer& tmr)
{
    if (conn_list_.empty() && !inprogress_) {
        SWIRLY_INFO << "reconnecting";
        if (!connect(now, reactor_, ep_)) {
            inprogress_ = true;
        }
    }
}

} // namespace fix
} // namespace swirly
