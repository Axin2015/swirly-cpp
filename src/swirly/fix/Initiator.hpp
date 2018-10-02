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
#ifndef SWIRLY_FIX_INITIATOR_HPP
#define SWIRLY_FIX_INITIATOR_HPP

#include <swirly/fix/Conn.hpp>

#include <swirly/sys/TcpConnector.hpp>

namespace swirly {
inline namespace fix {

class SWIRLY_API FixInitiator : public TcpConnector<FixInitiator> {

    friend TcpConnector<FixInitiator>;
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption
        = boost::intrusive::member_hook<FixConn, decltype(FixConn::list_hook), &FixConn::list_hook>;
    using ConnList = boost::intrusive::list<FixConn, ConstantTimeSizeOption, MemberHookOption>;

  public:
    FixInitiator(CyclTime now, Reactor& r, const Endpoint& ep, FixSessMap::node_type&& node,
                 FixAppBase& app);
    ~FixInitiator();

  private:
    void do_connect(CyclTime now, IoSocket&& sock, const Endpoint& ep);
    void do_connect_error(CyclTime now, const std::exception& e);
    void on_timer(CyclTime now, Timer& tmr);

    Reactor& reactor_;
    const Endpoint ep_;
    FixAppBase& app_;
    // Singleton map for initiator.
    FixSessMap sess_map_;
    Timer tmr_;
    bool inprogress_{false};
    // List of active connections.
    ConnList conn_list_;
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_INITIATOR_HPP
