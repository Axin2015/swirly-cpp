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
#include "HttpSess.hpp"

#include "RestServ.hpp"

namespace swirly {
using namespace std;

namespace {

constexpr auto IdleTimeout = 5s;
enum { MaxData = 2048 };

} // namespace

HttpSess::HttpSess(Time now, Reactor& r, IoSocket&& sock, const TcpEndpoint& ep, RestServ& rs)
: BasicHttpParser<HttpSess>{HttpType::Request}
, reactor_(r)
, sock_{move(sock)}
, ep_{ep}
, rest_serv_(rs)
{
    SWIRLY_DEBUG << "accept session";

    sub_ = r.subscribe(*sock_, EventIn, bind<&HttpSess::on_io_event>(this));
    tmr_ = r.timer(now + IdleTimeout, Priority::Low, bind<&HttpSess::on_timer>(this));
}

HttpSess::~HttpSess() = default;

void HttpSess::dispose() noexcept
{
    SWIRLY_DEBUG << "close session";
    delete this;
}

bool HttpSess::on_url(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.append_url(sv);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "error handling url: " << e.what();
    }
    return ret;
}

bool HttpSess::on_header_field(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.append_header_field(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "error handling header field: " << e.what();
    }
    return ret;
}

bool HttpSess::on_header_value(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.append_header_value(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "error handling header value: " << e.what();
    }
    return ret;
}

bool HttpSess::on_headers_end() noexcept
{
    req_.set_method(method());
    return true;
}

bool HttpSess::on_body(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.append_body(sv);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "error handling body: " << e.what();
    }
    return ret;
}

bool HttpSess::on_message_end() noexcept
{
    bool ret{false};
    try {
        --pending_;
        req_.flush(); // May throw.

        const auto was_empty = buf_.empty();
        rest_serv_.handle_request(req_, os_);

        if (was_empty) {
            // May throw.
            sub_.set_events(EventIn | EventOut);
        }
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "error handling message: " << e.what();
    }
    req_.clear();
    return ret;
}

void HttpSess::on_io_event(Time now, int fd, unsigned events)
{
    try {
        if (events & EventOut) {
            buf_.consume(os::write(fd, buf_.data()));
            if (buf_.empty()) {
                if (should_keep_alive()) {
                    // May throw.
                    sub_.set_events(EventIn);
                } else {
                    dispose();
                }
            }
        }
        if (events & (EventIn | EventHup)) {
            char in[MaxData];
            const auto size = os::read(fd, in, sizeof(in));
            if (size > 0) {
                parse({in, size});
                tmr_.cancel();
                tmr_ = reactor_.timer(now + IdleTimeout, Priority::Low,
                                      bind<&HttpSess::on_timer>(this));
            } else {
                dispose();
            }
        }
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "error handling io event: " << e.what();
        dispose();
    }
}

void HttpSess::on_timer(Time now, Timer& tmr)
{
    SWIRLY_WARNING << "session timeout";
    dispose();
}

} // namespace swirly
