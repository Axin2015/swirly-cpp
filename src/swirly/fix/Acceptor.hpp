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
#ifndef SWIRLY_FIX_ACCEPTOR_HPP
#define SWIRLY_FIX_ACCEPTOR_HPP

#include <swirly/fix/Conn.hpp>

#include <swirly/sys/TcpAcceptor.hpp>

namespace swirly {
inline namespace fix {

class SWIRLY_API FixAcceptor : public TcpAcceptor<FixAcceptor> {

    friend TcpAcceptor<FixAcceptor>;
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption
        = boost::intrusive::member_hook<FixConn, decltype(FixConn::list_hook), &FixConn::list_hook>;
    using ConnList = boost::intrusive::list<FixConn, ConstantTimeSizeOption, MemberHookOption>;

  public:
    FixAcceptor(CyclTime now, Reactor& r, const Endpoint& ep, FixAppBase& app);
    ~FixAcceptor();

    // Copy.
    FixAcceptor(const FixAcceptor&) = delete;
    FixAcceptor& operator=(const FixAcceptor&) = delete;

    // Move.
    FixAcceptor(FixAcceptor&&) = delete;
    FixAcceptor& operator=(FixAcceptor&&) = delete;

    void insert(FixSessMap::node_type&& node);

  private:
    void do_accept(CyclTime now, IoSocket&& sock, const Endpoint& ep);
    Reactor& reactor_;
    FixAppBase& app_;
    FixSessMap sess_map_;
    // List of active connections.
    ConnList conn_list_;
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_ACCEPTOR_HPP
