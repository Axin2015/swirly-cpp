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

HttpSess::HttpSess(Reactor& r, IoSocket&& sock, const TcpEndpoint& ep, RestServ& rs, Time now)
: EventHandler{r}
, BasicHttpParser<HttpSess>{HttpType::Request}
, sock_{move(sock)}
, ep_{ep}
, restServ_(rs)
{
    SWIRLY_INFO(logMsg() << "accept session");

    const auto eh = self();
    sub_ = r.subscribe(*sock_, EventIn, eh);
    tmr_ = r.setTimer(now + IdleTimeout, Priority::Low, eh);
}

HttpSess::~HttpSess() noexcept = default;

void HttpSess::doClose() noexcept
{
    SWIRLY_INFO(logMsg() << "close session");
    pause();
    tmr_.cancel();
    sub_.reset();
}

void HttpSess::doReady(int fd, unsigned events, Time now)
{
    try {
        if (events & EventOut) {
            outbuf_.consume(os::write(fd, outbuf_.data()));
            if (outbuf_.empty()) {
                // May throw.
                reactor().setEvents(*sock_, EventIn);
            }
        }
        if (events & EventIn) {
            char in[MaxData];
            const auto size = os::read(fd, in, sizeof(in));
            if (size == 0) {
                close();
            } else {
                parse({in, size});
                tmr_.cancel();
                tmr_ = reactor().setTimer(now + IdleTimeout, Priority::Low, self());
            }
        }
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling url: " << e.what());
        close();
    }
}

void HttpSess::doTimer(const Timer& tmr, Time now)
{
    SWIRLY_INFO(logMsg() << "timeout");
    close();
}

bool HttpSess::doUrl(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.appendUrl(sv);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling url: " << e.what());
        close();
    }
    return ret;
}

bool HttpSess::doHeaderField(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.appendHeaderField(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling header field: " << e.what());
        close();
    }
    return ret;
}

bool HttpSess::doHeaderValue(string_view sv, bool first) noexcept
{
    bool ret{false};
    try {
        req_.appendHeaderValue(sv, first);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling header value: " << e.what());
        close();
    }
    return ret;
}

bool HttpSess::doHeadersEnd() noexcept
{
    req_.setMethod(method());
    return true;
}

bool HttpSess::doBody(string_view sv) noexcept
{
    bool ret{false};
    try {
        req_.appendBody(sv);
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling body: " << e.what());
        close();
    }
    return ret;
}

bool HttpSess::doMessageEnd() noexcept
{
    bool ret{false};
    try {
        req_.flush(); // May throw.

        const auto wasEmpty = outbuf_.empty();
        restServ_.handleRequest(req_, rsp_);

        if (wasEmpty) {
            // May throw.
            reactor().setEvents(*sock_, EventIn | EventOut);
        }
        ret = true;
    } catch (const std::exception& e) {
        SWIRLY_ERROR(logMsg() << "exception handling message: " << e.what());
        close();
    }
    req_.clear();
    return ret;
}

} // namespace swirly
