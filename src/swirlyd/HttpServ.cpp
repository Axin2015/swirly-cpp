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
#include "HttpServ.hpp"

#include "HttpSess.hpp"

using namespace boost;
using namespace std;

using asio::ip::tcp;

namespace swirly {

HttpServ::HttpServ(asio::io_service& ioServ, uint16_t port, RestServ& restServ)
  : ioServ_(ioServ)
  , acceptor_{ioServ}
  , restServ_(restServ)
{
    tcp::endpoint endpoint{tcp::v4(), port};
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address{true});
    acceptor_.bind(endpoint);
    acceptor_.listen();

    asyncAccept();
}

HttpServ::~HttpServ() noexcept = default;

void HttpServ::asyncAccept()
{
    auto sess = makeIntrusive<HttpSess>(ioServ_, restServ_);
    acceptor_.async_accept(sess->socket(), [this, sess](auto ec) {
        if (!ec) {
            sess->start();
        }
        this->asyncAccept();
    });
}

} // namespace swirly
