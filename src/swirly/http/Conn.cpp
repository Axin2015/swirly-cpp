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

#include <swirly/sys/Event.hpp>

namespace swirly {
inline namespace http {
using namespace std;
namespace {
constexpr auto IdleTimeout = 5s;
} // namespace

HttpConn::HttpConn(Time now, Reactor& r, IoSocket&& sock, const Endpoint& ep, HttpApp& app)
: BasicHttpParser<HttpConn>{HttpType::Request}
, reactor_(r)
, sock_{move(sock)}
, ep_{ep}
, app_(app)
{
    sub_ = r.subscribe(*sock_, EventIn, bind<&HttpConn::on_io_event>(this));
    tmr_ = r.timer(now + IdleTimeout, Priority::Low, bind<&HttpConn::on_timer>(this));
    app.on_connect(now, *this);
}

void HttpConn::clear() noexcept
{
    req_.clear();
}

void HttpConn::dispose(Time now) noexcept
{
    app_.on_disconnect(now, *this);
    delete this;
}

HttpConn::~HttpConn() = default;

bool HttpConn::on_url(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.append_url(sv);
        ret = true;
    } catch (const std::exception& e) {
        app_.on_error(now_, *this, e);
    }
    return ret;
}

bool HttpConn::on_header_field(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.append_header_field(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        app_.on_error(now_, *this, e);
    }
    return ret;
}

bool HttpConn::on_header_value(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.append_header_value(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        app_.on_error(now_, *this, e);
    }
    return ret;
}

bool HttpConn::on_headers_end() noexcept
{
    req_.set_method(method());
    return true;
}

bool HttpConn::on_body(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.append_body(sv);
        ret = true;
    } catch (const std::exception& e) {
        app_.on_error(now_, *this, e);
    }
    return ret;
}

bool HttpConn::on_message_end() noexcept
{
    bool ret{false};
    try {
        --pending_;
        req_.flush(); // May throw.

        const auto was_empty = out_.empty();
        app_.on_message(now_, *this, req_, os_);

        if (was_empty) {
            // May throw.
            sub_.set_events(EventIn | EventOut);
        }
        ret = true;
    } catch (const std::exception& e) {
        app_.on_error(now_, *this, e);
    }
    req_.clear();
    return ret;
}

void HttpConn::on_io_event(Time now, int fd, unsigned events)
{
    try {
        if (events & EventOut) {
            out_.consume(os::write(fd, out_.data()));
            if (out_.empty()) {
                if (should_keep_alive()) {
                    // May throw.
                    sub_.set_events(EventIn);
                } else {
                    dispose(now);
                }
            }
        }
        if (events & (EventIn | EventHup)) {
            const auto size = os::read(fd, in_.prepare(2048));
            if (size > 0) {
                // Commit actual bytes read.
                in_.commit(size);

                now_ = now;
                in_.consume(parse(in_.data()));

                // Reset timer.
                tmr_.cancel();
                tmr_ = reactor_.timer(now + IdleTimeout, Priority::Low,
                                      bind<&HttpConn::on_timer>(this));
            } else {
                dispose(now);
            }
        }
    } catch (const ProtocolException&) {
        // Do not call on_error() here, because it will have already been called in one of the
        // noexcept parser callback functions.
        dispose(now);
    } catch (const std::exception& e) {
        app_.on_error(now_, *this, e);
        dispose(now);
    }
}

void HttpConn::on_timer(Time now, Timer& tmr)
{
    app_.on_timeout(now, *this);
    dispose(now);
}

} // namespace http
} // namespace swirly
