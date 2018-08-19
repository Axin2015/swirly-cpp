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

#include <swirly/fix/Sess.hpp>

#include <swirly/sys/TcpConnector.hpp>

namespace swirly {
inline namespace fix {

class SWIRLY_API FixInitiator : public TcpConnector<FixInitiator> {

    friend TcpConnector<FixInitiator>;
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using MemberHookOption
        = boost::intrusive::member_hook<FixSess, decltype(FixSess::list_hook), &FixSess::list_hook>;
    using SessList = boost::intrusive::list<FixSess, ConstantTimeSizeOption, MemberHookOption>;

  public:
    FixInitiator(Reactor& r, const Endpoint& ep, const FixConfig& config, const FixSessId& sess_id,
                 FixApp& app, Time now);
    ~FixInitiator();

  private:
    void do_connect(IoSocket&& sock, const Endpoint& ep, Time now);
    void do_connect_error(const std::exception& e, Time now);
    void on_timer(Timer& tmr, Time now);

    Reactor& reactor_;
    const Endpoint ep_;
    const FixConfig& config_;
    const FixSessId sess_id_;
    FixApp& app_;
    Timer tmr_;
    bool inprogress_{false};
    // List of active sessions.
    SessList sess_list_;
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_INITIATOR_HPP
