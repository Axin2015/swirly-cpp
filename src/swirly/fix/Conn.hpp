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
#ifndef SWIRLY_FIX_CONN_HPP
#define SWIRLY_FIX_CONN_HPP

#include <swirly/fix/Config.hpp>
#include <swirly/fix/Parser.hpp>

#include <swirly/sys/IoSocket.hpp>
#include <swirly/sys/Reactor.hpp>

#include <boost/intrusive/list.hpp>

namespace swirly {
inline namespace fix {

class FixApp;
struct FixConfig;

class SWIRLY_API FixConn final : BasicFixParser<FixConn> {
    friend class BasicFixParser<FixConn>;

    // Automatically unlink when object is destroyed.
    using AutoUnlinkOption = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    FixConn(Time now, Reactor& r, IoSocket&& sock, const Endpoint& ep, const FixSessMap& sess_map,
            FixApp& app);
    const Endpoint& endpoint() const noexcept { return ep_; }
    int seq_num() const noexcept { return seq_num_; }
    void logon(Time now, const FixSessId& sess_id);
    void logout(Time now);
    boost::intrusive::list_member_hook<AutoUnlinkOption> list_hook;

  private:
    void dispose(Time now) noexcept;
    void read_and_write(Time now);
    void read_only(Time now);
    void send_logon(Time now);
    void send_logout(Time now);
    void on_io_event(Time now, int fd, unsigned events);
    void schedule_timeout(Time now);
    void on_timeout(Time now, Timer& tmr);
    void schedule_heartbeat(Time now);
    void on_heartbeat(Time now, Timer& tmr);
    void on_message(Time now, std::string_view msg, std::size_t msg_type_off, Version ver);
    Reactor& reactor_;
    IoSocket sock_;
    const Endpoint ep_;
    const FixSessMap& sess_map_;
    FixSessId sess_id_;
    // Agreed upon by the two firms and specified by the Logon initiator and echoed back by the
    // Logon acceptor.
    Seconds hb_int_{2s};
    FixApp& app_;
    enum { LoggedOut, LogonSent, LoggedOn, LogoutSent } state_{LoggedOut};
    Reactor::Handle sub_;
    struct {
        Buffer buf;
        Timer tmr;
    } in_;
    struct {
        Buffer buf;
        Timer tmr;
    } out_;
    int seq_num_{0};
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_CONN_HPP