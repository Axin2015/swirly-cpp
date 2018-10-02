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
#ifndef SWIRLY_HTTP_SERV_HPP
#define SWIRLY_HTTP_SERV_HPP

#include <swirly/http/App.hpp>
#include <swirly/http/Conn.hpp>

#include <swirly/sys/TcpAcceptor.hpp>

namespace swirly {
inline namespace http {

template <typename ConnT, typename AppT>
class BasicHttpServ : public TcpAcceptor<BasicHttpServ<ConnT, AppT>> {

    friend TcpAcceptor<BasicHttpServ<ConnT, AppT>>;

    using Conn = ConnT;
    using App = AppT;
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption
        = boost::intrusive::member_hook<Conn, decltype(Conn::list_hook), &Conn::list_hook>;
    using ConnList = boost::intrusive::list<Conn, ConstantTimeSizeOption, MemberHookOption>;

    using typename TcpAcceptor<BasicHttpServ<ConnT, AppT>>::Endpoint;

  public:
    BasicHttpServ(CyclTime now, Reactor& r, const Endpoint& ep, App& app)
    : TcpAcceptor<BasicHttpServ<ConnT, AppT>>{r, ep}
    , reactor_(r)
    , app_(app)
    {
    }
    ~BasicHttpServ()
    {
        const auto now = CyclTime::current();
        conn_list_.clear_and_dispose([now](auto* conn) { conn->dispose(now); });
    }

    // Copy.
    BasicHttpServ(const BasicHttpServ&) = delete;
    BasicHttpServ& operator=(const BasicHttpServ&) = delete;

    // Move.
    BasicHttpServ(BasicHttpServ&&) = delete;
    BasicHttpServ& operator=(BasicHttpServ&&) = delete;

  private:
    void do_accept(CyclTime now, IoSocket&& sock, const Endpoint& ep)
    {
        auto* const conn = new Conn{now, reactor_, std::move(sock), ep, app_};
        conn_list_.push_back(*conn);
    }

    Reactor& reactor_;
    App& app_;
    // List of active connections.
    ConnList conn_list_;
};

using HttpServ = BasicHttpServ<HttpConn, HttpAppBase>;

} // namespace http
} // namespace swirly

#endif // SWIRLY_HTTP_SERV_HPP
