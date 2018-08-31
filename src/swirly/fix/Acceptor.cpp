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
#include "Acceptor.hpp"

namespace swirly {
inline namespace fix {
using namespace std;

FixAcceptor::FixAcceptor(Time now, Reactor& r, const Endpoint& ep, const FixConfig& config,
                         FixApp& app)
: TcpAcceptor{r, ep}
, reactor_(r)
, config_(config)
, app_(app)
{
}

FixAcceptor::~FixAcceptor()
{
    sess_list_.clear_and_dispose([](auto* sess) { delete sess; });
}

void FixAcceptor::do_accept(Time now, IoSocket&& sock, const Endpoint& ep)
{
    sock.set_non_block();
    sock.set_tcp_no_delay(true);
    // High performance TCP servers could use a custom allocator.
    auto* const sess = new FixSess{now, reactor_, move(sock), ep, config_, app_};
    sess_list_.push_back(*sess);
}

} // namespace fix
} // namespace swirly
