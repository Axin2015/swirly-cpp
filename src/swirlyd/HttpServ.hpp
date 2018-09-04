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
#ifndef SWIRLYD_HTTPSERV_HPP
#define SWIRLYD_HTTPSERV_HPP

#include "HttpConn.hpp"

#include <swirly/sys/TcpAcceptor.hpp>

namespace swirly {

class RestServ;

class SWIRLY_API HttpServ : public TcpAcceptor<HttpServ> {
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption = boost::intrusive::member_hook<HttpConn, decltype(HttpConn::list_hook),
                                                           &HttpConn::list_hook>;
    using ConnList = boost::intrusive::list<HttpConn, ConstantTimeSizeOption, MemberHookOption>;

  public:
    HttpServ(Reactor& r, const Endpoint& ep, RestServ& rs);
    ~HttpServ();

    // Copy.
    HttpServ(const HttpServ&) = delete;
    HttpServ& operator=(const HttpServ&) = delete;

    // Move.
    HttpServ(HttpServ&&) = delete;
    HttpServ& operator=(HttpServ&&) = delete;

    void do_accept(Time now, IoSocket&& sock, const Endpoint& ep);

  private:
    Reactor& reactor_;
    RestServ& rest_serv_;
    // List of active connections.
    ConnList conn_list_;
};

} // namespace swirly

#endif // SWIRLYD_HTTPSERV_HPP
