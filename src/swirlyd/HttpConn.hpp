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
#ifndef SWIRLYD_HTTPCONN_HPP
#define SWIRLYD_HTTPCONN_HPP

#include "HttpRequest.hpp"

#include <swirly/http/Parser.hpp>
#include <swirly/http/Stream.hpp>

#include <swirly/app/MemAlloc.hpp>

#include <swirly/sys/IoSocket.hpp>
#include <swirly/sys/IpAddress.hpp>
#include <swirly/sys/Reactor.hpp>

#include <swirly/util/Log.hpp>

#include <boost/intrusive/list.hpp>

namespace swirly {
class RestApp;

class SWIRLY_API HttpConn
: public MemAlloc
, BasicHttpParser<HttpConn> {

    friend class BasicHttpParser<HttpConn>;

    // Automatically unlink when object is destroyed.
    using AutoUnlinkOption = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    HttpConn(Time now, Reactor& r, IoSocket&& sock, const Endpoint& ep, RestApp& app);

    // Copy.
    HttpConn(const HttpConn&) = delete;
    HttpConn& operator=(const HttpConn&) = delete;

    // Move.
    HttpConn(HttpConn&&) = delete;
    HttpConn& operator=(HttpConn&&) = delete;

    const Endpoint& endpoint() const noexcept { return ep_; }
    void dispose(Time now) noexcept;

    boost::intrusive::list_member_hook<AutoUnlinkOption> list_hook;

  private:
    ~HttpConn();
    bool on_message_begin() noexcept
    {
        ++pending_;
        return true;
    }
    bool on_url(std::string_view sv) noexcept;
    bool on_status(std::string_view sv) noexcept
    {
        // Only supported for HTTP responses.
        return false;
    }
    bool on_header_field(std::string_view sv, bool first) noexcept;
    bool on_header_value(std::string_view sv, bool first) noexcept;
    bool on_headers_end() noexcept;
    bool on_body(std::string_view sv) noexcept;
    bool on_message_end() noexcept;
    bool on_chunk_header(std::size_t len) noexcept { return true; }
    bool on_chunk_end() noexcept { return true; }

    void on_io_event(Time now, int fd, unsigned events);
    void on_timer(Time now, Timer& tmr);

    LogMsg& log_msg() const noexcept
    {
        auto& ref = swirly::log_msg();
        ref << '<' << ep_ << "> ";
        return ref;
    }

    Reactor& reactor_;
    IoSocket sock_;
    Endpoint ep_;
    RestApp& app_;
    Reactor::Handle sub_;
    Timer tmr_;
    Time now_{};
    int pending_{0};
    Buffer in_, out_;
    HttpRequest req_;
    HttpStream os_{out_};
};

} // namespace swirly

#endif // SWIRLYD_HTTPCONN_HPP
