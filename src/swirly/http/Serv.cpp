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
#include "Serv.hpp"

namespace swirly {
inline namespace http {
using namespace std;

HttpServ::HttpServ(Time now, Reactor& r, const Endpoint& ep, HttpApp& app)
: TcpAcceptor{r, ep}
, reactor_(r)
, app_(app)
{
}

HttpServ::~HttpServ()
{
    const auto now = UnixClock::now();
    conn_list_.clear_and_dispose([now](auto* conn) { conn->dispose(now); });
}

void HttpServ::do_accept(Time now, IoSocket&& sock, const Endpoint& ep)
{
    auto* const conn = new HttpConn{now, reactor_, move(sock), ep, app_};
    conn_list_.push_back(*conn);
}

} // namespace http
} // namespace swirly
