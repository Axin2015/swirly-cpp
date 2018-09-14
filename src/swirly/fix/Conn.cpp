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
#include "Conn.hpp"

#include "App.hpp"
#include "Header.hpp"
#include "Lexer.hpp"
#include "Logon.hpp"
#include "Stream.hpp"

#include <swirly/sys/Event.hpp>

namespace swirly {
inline namespace fix {
using namespace std;

FixConn::FixConn(Time now, Reactor& r, IoSocket&& sock, const Endpoint& ep,
                 const FixSessMap& sess_map, FixApp& app)
: reactor_(r)
, sock_{move(sock)}
, ep_{ep}
, sess_map_(sess_map)
, app_(app)
{
    sub_ = r.subscribe(sock_.get(), EventIn, bind<&FixConn::on_io_event>(this));
    schedule_timeout(now);
    app.on_connect(now, *this);
}

void FixConn::dispose(Time now) noexcept
{
    app_.on_disconnect(now, *this);
    delete this;
}

void FixConn::logon(Time now, const FixSessId& sess_id)
{
    if (state_ == LoggedOut) {
        const auto it = sess_map_.find(sess_id);
        if (it == sess_map_.end()) {
            throw std::runtime_error{"invalid session: "s + to_string(sess_id)};
        }
        state_ = LogonSent;
        sess_id_ = sess_id;
        hb_int_ = Seconds{it->second.get<int>("heart_bt_int")};
        send_logon(now);
    }
}

void FixConn::logout(Time now)
{
    if (state_ == LoggedOn) {
        // Initiate logout.
        send_logout(now);
        state_ = LogoutSent;
    }
}

FixConn::~FixConn() = default;

void FixConn::read_and_write(Time now)
{
    sub_.set_events(EventIn | EventOut);
    out_.tmr.reset();
}

void FixConn::read_only(Time now)
{
    sub_.set_events(EventIn);
    if (state_ == LoggedOn) {
        schedule_heartbeat(now);
    }
}

void FixConn::send_logon(Time now)
{
    FixHeader hdr;
    hdr.msg_type = "A"sv;
    hdr.sender_comp_id = sess_id_.sender_comp_id;
    hdr.target_comp_id = sess_id_.target_comp_id;
    hdr.msg_seq_num = ++seq_num_;
    hdr.sending_time = now;

    Logon body{0, hb_int_.count()};

    FixStream os{out_.buf};
    os.reset(sess_id_.version);
    os << hdr << body;
    os.commit();

    read_and_write(now);
}

void FixConn::send_logout(Time now)
{
    FixHeader hdr;
    hdr.msg_type = "5"sv;
    hdr.sender_comp_id = sess_id_.sender_comp_id;
    hdr.target_comp_id = sess_id_.target_comp_id;
    hdr.msg_seq_num = ++seq_num_;
    hdr.sending_time = now;

    FixStream os{out_.buf};
    os.reset(sess_id_.version);
    os << hdr;
    os.commit();

    read_and_write(now);
}

void FixConn::on_io_event(Time now, int fd, unsigned events)
{
    try {
        if (events & EventOut) {
            out_.buf.consume(os::write(fd, out_.buf.data()));
            if (out_.buf.empty()) {
                // May throw.
                read_only(now);
            }
        }
        if (events & (EventIn | EventHup)) {
            const auto size = os::read(fd, in_.buf.prepare(2048));
            if (size > 0) {
                // Commit actual bytes read.
                in_.buf.commit(size);
                in_.buf.consume(parse(now, in_.buf.data()));
                schedule_timeout(now);
            } else {
                dispose(now);
            }
        }
    } catch (const std::exception& e) {
        app_.on_error(now, *this, e);
        dispose(now);
    }
}

void FixConn::schedule_timeout(Time now)
{
    in_.tmr = reactor_.timer(now + hb_int_ + 1s, Priority::Low, bind<&FixConn::on_timeout>(this));
}

void FixConn::on_timeout(Time now, Timer& tmr)
{
    app_.on_timeout(now, *this);
    dispose(now);
}

void FixConn::schedule_heartbeat(Time now)
{
    out_.tmr = reactor_.timer(now + hb_int_, Priority::Low, bind<&FixConn::on_heartbeat>(this));
}

void FixConn::on_heartbeat(Time now, Timer& tmr)
{
    FixHeader hdr;
    hdr.msg_type = "0"sv;
    hdr.sender_comp_id = sess_id_.sender_comp_id;
    hdr.target_comp_id = sess_id_.target_comp_id;
    hdr.msg_seq_num = ++seq_num_;
    hdr.sending_time = now;

    FixStream os{out_.buf};
    os.reset(sess_id_.version);
    os << hdr;
    os.commit();

    read_and_write(now);
}

void FixConn::on_message(Time now, std::string_view msg, std::size_t msg_type_off, Version ver)
{
    FixHeader hdr;
    const size_t body_off = parse_header(msg, msg_type_off, hdr);
    const auto& msg_type = hdr.msg_type.value;
    if (msg_type.empty()) {
        throw domain_error{"invalid FIX header"};
    }
    if (msg_type == "0"sv) {
        // Heartbeat.
    } else if (msg_type == "1"sv) {
        // Test Request.
    } else if (msg_type == "2"sv) {
        // Resend Request.
    } else if (msg_type == "3"sv) {
        // Reject.
    } else if (msg_type == "4"sv) {
        // Sequence Reset.
    } else if (msg_type == "5"sv) {
        // Logout.
        if (state_ == LogoutSent) {
            state_ = LoggedOut;
            // Process logout reply.
            FixSessId sess_id;
            sess_id_.swap(sess_id);
            app_.on_logout(now, *this, sess_id);
        } else if (state_ == LoggedOn) {
            state_ = LoggedOut;
            FixSessId sess_id;
            sess_id_.swap(sess_id);
            // Send logout response.
            send_logout(now);
            app_.on_logout(now, *this, sess_id);
        }
    } else if (msg_type == "A"sv) {
        // Logon.
        if (state_ == LogonSent) {
            // Process logon reply.
            state_ = LoggedOn;
            schedule_heartbeat(now);
            app_.on_logon(now, *this, sess_id_);
        } else if (state_ == LoggedOut) {
            state_ = LoggedOn;
            Logon body;
            parse_body(msg, body_off, body);
            // Send logon response.
            sess_id_ = ~get_sess_id<string>(ver, hdr);
            hb_int_ = Seconds{body.heart_bt_int.value};
            send_logon(now);
            app_.on_logon(now, *this, sess_id_);
        }
    }
    app_.on_message(now, *this, msg, body_off, ver, hdr);
}

} // namespace fix
} // namespace swirly
