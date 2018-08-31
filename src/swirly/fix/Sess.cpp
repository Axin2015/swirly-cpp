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
#include "Sess.hpp"

#include "App.hpp"
#include "Config.hpp"
#include "Hdr.hpp"
#include "Lexer.hpp"
#include "Logon.hpp"
#include "Stream.hpp"

#include <swirly/sys/Event.hpp>

#include <swirly/util/Log.hpp>

namespace swirly {
inline namespace fix {
using namespace std;

FixSess::FixSess(Time now, Reactor& r, IoSocket&& sock, const Endpoint& ep, const FixConfig& config,
                 FixApp& app)
: reactor_(r)
, sock_{move(sock)}
, ep_{ep}
, config_(config)
, app_(app)
{
    sub_ = r.subscribe(sock_.get(), EventIn, bind<&FixSess::on_io_event>(this));
    schedule_timeout(now);
    app.on_connect(now, *this);
}

void FixSess::logon(Time now, const FixSessId& sess_id)
{
    if (state_ == LoggedOut) {
        sess_id_ = sess_id;
        const auto it = config_.sess_map.find(sess_id);
        if (it == config_.sess_map.end()) {
            throw std::runtime_error{"invalid session: "s + to_string(sess_id)};
        }
        hb_int_ = Seconds{it->second.get<int>("heart_bt_int")};
        send_logon(now);
        state_ = LogonSent;
    }
}

void FixSess::logout(Time now)
{
    if (state_ == LoggedOn) {
        // Initiate logout.
        send_logout(now);
        state_ = LogoutSent;
    }
}

void FixSess::dispose(Time now) noexcept
{
    delete this;
}

void FixSess::read_and_write(Time now)
{
    sub_.set_events(EventIn | EventOut);
    out_.tmr.reset();
}

void FixSess::read_only(Time now)
{
    sub_.set_events(EventIn);
    if (state_ == LoggedOn) {
        schedule_heartbeat(now);
    }
}

void FixSess::send_logon(Time now)
{
    FixHdr hdr;
    hdr.msg_type = "A"sv;
    hdr.sender_comp_id = sess_id_.sender_comp_id;
    hdr.target_comp_id = sess_id_.target_comp_id;
    hdr.msg_seq_num = ++seq_num_;
    hdr.sending_time = UnixClock::now();

    Logon body{0, hb_int_.count()};

    FixStream os{out_.buf};
    os.reset(sess_id_.version);
    os << hdr << body;
    os.commit();

    read_and_write(now);
}

void FixSess::send_logout(Time now)
{
    FixHdr hdr;
    hdr.msg_type = "5"sv;
    hdr.sender_comp_id = sess_id_.sender_comp_id;
    hdr.target_comp_id = sess_id_.target_comp_id;
    hdr.msg_seq_num = ++seq_num_;
    hdr.sending_time = UnixClock::now();

    FixStream os{out_.buf};
    os.reset(sess_id_.version);
    os << hdr;
    os.commit();

    read_and_write(now);
}

void FixSess::on_io_event(Time now, int fd, unsigned events)
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
                app_.on_disconnect(now, *this);
                dispose(now);
            }
        }
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "error handling io event: " << e.what();
        dispose(now);
    }
}

void FixSess::schedule_timeout(Time now)
{
    in_.tmr = reactor_.timer(now + hb_int_ + 1s, Priority::Low, bind<&FixSess::on_timeout>(this));
}

void FixSess::on_timeout(Time now, Timer& tmr)
{
    SWIRLY_WARNING << "session timeout";
    dispose(now);
}

void FixSess::schedule_heartbeat(Time now)
{
    out_.tmr = reactor_.timer(now + hb_int_, Priority::Low, bind<&FixSess::on_heartbeat>(this));
}

void FixSess::on_heartbeat(Time now, Timer& tmr)
{
    FixHdr hdr;
    hdr.msg_type = "0"sv;
    hdr.sender_comp_id = sess_id_.sender_comp_id;
    hdr.target_comp_id = sess_id_.target_comp_id;
    hdr.msg_seq_num = ++seq_num_;
    hdr.sending_time = UnixClock::now();

    FixStream os{out_.buf};
    os.reset(sess_id_.version);
    os << hdr;
    os.commit();

    read_and_write(now);
}

void FixSess::on_message(Time now, std::string_view msg, std::size_t msg_type_off, Version ver)
{
    FixHdr hdr;
    const size_t body_off = parse_hdr(msg, msg_type_off, hdr);
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
            // Process logout reply.
            app_.on_logout(now, *this);
            state_ = LoggedOut;
        } else if (state_ == LoggedOn) {
            // Send logout response.
            send_logout(now);
            app_.on_logout(now, *this);
            state_ = LoggedOut;
        }
    } else if (msg_type == "A"sv) {
        // Logon.
        if (state_ == LogonSent) {
            // Process logon reply.
            app_.on_logon(now, *this);
            state_ = LoggedOn;
            schedule_heartbeat(now);
        } else if (state_ == LoggedOut) {
            Logon body;
            parse_body(msg, body_off, body);
            // Send logon response.
            sess_id_ = ~get_sess_id<string>(ver, hdr);
            hb_int_ = Seconds{body.heart_bt_int.value};
            send_logon(now);
            app_.on_logon(now, *this);
            state_ = LoggedOn;
        }
    }
    app_.on_message(now, *this, msg, body_off, ver, hdr);
}

} // namespace fix
} // namespace swirly
