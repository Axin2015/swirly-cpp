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

#include <swirly/web/Parser.hpp>
#include <swirly/web/Request.hpp>
#include <swirly/web/Stream.hpp>

#include <swirly/app/MemAlloc.hpp>

#include <swirly/sys/TcpAcceptor.hpp>

#include <swirly/util/Log.hpp>

#include <boost/intrusive/list.hpp>

namespace swirly {

class RestServ;

class SWIRLY_API HttpConn
: public MemAlloc
, BasicHttpParser<HttpConn> {
    friend class BasicHttpParser<HttpConn>;
    using AutoUnlinkOption = boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

  public:
    HttpConn(Time now, Reactor& r, IoSocket&& sock, const TcpEndpoint& ep, RestServ& rs);

    // Copy.
    HttpConn(const HttpConn&) = delete;
    HttpConn& operator=(const HttpConn&) = delete;

    // Move.
    HttpConn(HttpConn&&) = delete;
    HttpConn& operator=(HttpConn&&) = delete;

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
    bool on_chunk_header(size_t len) noexcept { return true; }
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
    TcpEndpoint ep_;
    RestServ& rest_serv_;
    Reactor::Handle sub_;
    Timer tmr_;
    int pending_{0};
    HttpRequest req_;
    Buffer buf_;
    HttpStream os_{buf_};
};

} // namespace swirly

#endif // SWIRLYD_HTTPCONN_HPP
