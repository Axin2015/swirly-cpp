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
#include <swirly/fix/Header.hpp>
#include <swirly/fix/Parser.hpp>
#include <swirly/fix/Stream.hpp>

#include <swirly/sys/IoSocket.hpp>
#include <swirly/sys/Reactor.hpp>

#include <boost/intrusive/list.hpp>

namespace swirly {
inline namespace fix {

class FixAppBase;
struct FixConfig;

class SWIRLY_API FixConn final : BasicFixParser<FixConn> {
    friend class BasicFixParser<FixConn>;

    // Automatically unlink when object is destroyed.
    using AutoUnlinkOption = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    FixConn(CyclTime now, Reactor& r, IoSocket&& sock, const Endpoint& ep,
            const FixSessMap& sess_map, FixAppBase& app);

    // Copy.
    FixConn(const FixConn&) = delete;
    FixConn& operator=(const FixConn&) = delete;

    // Move.
    FixConn(FixConn&&) = delete;
    FixConn& operator=(FixConn&&) = delete;

    const Endpoint& endpoint() const noexcept { return ep_; }
    const FixSessId& sess_id() const noexcept { return sess_id_; }
    int seq_num() const noexcept { return seq_num_; }
    void dispose(CyclTime now) noexcept;
    void logon(CyclTime now, const FixSessId& sess_id);
    void logout(CyclTime now);
    void send(CyclTime now, std::string_view msg_type, std::string_view body);
    template <typename FnT>
    void send(CyclTime now, std::string_view msg_type, FnT fn)
    {
        const auto hdr = make_header(now, msg_type);

        FixStream os{out_.buf};
        os.reset(sess_id_.version);
        fn(now, os << hdr);
        os.commit();

        seq_num_ = get<Tag::MsgSeqNum>(hdr);
        read_and_write(now);
    }
    boost::intrusive::list_member_hook<AutoUnlinkOption> list_hook;

  private:
    ~FixConn();
    FixHeaderView make_header(CyclTime now, std::string_view msg_type) const noexcept;
    void read_and_write(CyclTime now);
    void read_only(CyclTime now);
    void send_logon(CyclTime now);
    void send_logout(CyclTime now);
    void on_io_event(CyclTime now, int fd, unsigned events);
    void schedule_timeout(CyclTime now);
    void on_timeout(CyclTime now, Timer& tmr);
    void schedule_heartbeat(CyclTime now);
    void on_heartbeat(CyclTime now, Timer& tmr);
    void on_message(CyclTime now, std::string_view msg, std::size_t msg_type_off, Version ver);

    Reactor& reactor_;
    IoSocket sock_;
    const Endpoint ep_;
    const FixSessMap& sess_map_;
    FixAppBase& app_;
    enum { LoggedOut, LogonSent, LoggedOn, LogoutSent } state_{LoggedOut};

    FixSessId sess_id_;
    // Agreed upon by the two firms and specified by the Logon initiator and echoed back by the
    // Logon acceptor.
    Seconds hb_int_{2s};
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
