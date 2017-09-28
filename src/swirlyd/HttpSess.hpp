/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLYD_HTTPSESS_HPP
#define SWIRLYD_HTTPSESS_HPP

#include "HttpRequest.hpp"

#include <swirly/web/HttpHandler.hpp>

#include <swirly/util/Log.hpp>
#include <swirly/util/RefCount.hpp>
#include <swirly/util/RingBuffer.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/asio.hpp>
#pragma GCC diagnostic pop

namespace swirly {

class HttpResponse;
class RestServ;

class HttpSess : public RefCount<HttpSess, ThreadUnsafePolicy>, public BasicHttpHandler<HttpSess> {

    friend class BasicHttpHandler<HttpSess>;
    enum { IdleTimeout = 5, MaxData = 4096 };

  public:
    HttpSess(boost::asio::io_service& ioServ, RestServ& restServ)
        : BasicHttpHandler<HttpSess>{HttpType::Request},
          sock_{ioServ},
          timeout_{ioServ},
          restServ_(restServ)
    {
    }
    ~HttpSess() noexcept;

    // Copy.
    HttpSess(const HttpSess&) = delete;
    HttpSess& operator=(const HttpSess&) = delete;

    // Move.
    HttpSess(HttpSess&&) = delete;
    HttpSess& operator=(HttpSess&&) = delete;

    LogMsg& logMsg() noexcept
    {
        auto& ref = swirly::logMsg();
        boost::system::error_code ec;
        ref << '<' << sock_.remote_endpoint(ec) << "> ";
        return ref;
    }

    void start();
    void stop() noexcept;
    auto& socket() noexcept { return sock_; }

  private:
    void parse();
    void resetTimeout();

    void asyncReadSome();
    void asyncWrite();
    void onReadSome(std::size_t len) noexcept;
    void onWrite() noexcept;

    bool onMessageBegin() noexcept { return true; }
    bool onUrl(std::string_view sv) noexcept;
    bool onStatus(std::string_view sv) noexcept
    {
        // Only supported for HTTP responses.
        return false;
    }
    bool onHeaderField(std::string_view sv, bool first) noexcept;
    bool onHeaderValue(std::string_view sv, bool first) noexcept;
    bool onHeadersEnd() noexcept;
    bool onBody(std::string_view sv) noexcept;
    bool onMessageEnd() noexcept;
    bool onChunkHeader(size_t len) noexcept { return true; }
    bool onChunkEnd() noexcept { return true; }

    boost::asio::ip::tcp::socket sock_;
    // Close session if client is inactive.
    boost::asio::deadline_timer timeout_;
    RestServ& restServ_;
    char data_[MaxData];
    boost::asio::const_buffer inbuf_;
    HttpRequest req_;
    RingBuffer<std::string> outbuf_{8};
};

using HttpSessPtr = boost::intrusive_ptr<HttpSess>;

} // swirly

#endif // SWIRLYD_HTTPSESS_HPP
