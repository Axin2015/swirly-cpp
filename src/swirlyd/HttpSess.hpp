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
#ifndef SWIRLYD_HTTPSESS_HPP
#define SWIRLYD_HTTPSESS_HPP

#include <swirly/web/HttpParser.hpp>
#include <swirly/web/HttpRequest.hpp>
#include <swirly/web/HttpResponse.hpp>

#include <swirly/util/Log.hpp>

#include <swirly/sys/MemAlloc.hpp>
#include <swirly/sys/TcpAcceptor.hpp>

namespace swirly {

class RestServ;

class SWIRLY_API HttpSess
  : public EventHandler
  , public BasicHttpParser<HttpSess>
  , public MemAlloc {
    friend class BasicHttpParser<HttpSess>;

  public:
    HttpSess(Reactor& r, IoSocket&& sock, const TcpEndpoint& ep, RestServ& rs, Time now);
    ~HttpSess() noexcept override;

    // Copy.
    HttpSess(const HttpSess&) = delete;
    HttpSess& operator=(const HttpSess&) = delete;

    // Move.
    HttpSess(HttpSess&&) = delete;
    HttpSess& operator=(HttpSess&&) = delete;

    static auto make(Reactor& r, IoSocket&& sock, const TcpEndpoint& ep, RestServ& rs, Time now)
    {
        return makeIntrusive<HttpSess>(r, std::move(sock), ep, rs, now);
    }

  protected:
    void doClose() noexcept override;
    void doReady(int fd, FileEvents events, Time now) override;
    void doTimer(const Timer& tmr, Time now) override;

    bool doMessageBegin() noexcept { return true; }
    bool doUrl(std::string_view sv) noexcept;
    bool doStatus(std::string_view sv) noexcept
    {
        // Only supported for HTTP responses.
        return false;
    }
    bool doHeaderField(std::string_view sv, bool first) noexcept;
    bool doHeaderValue(std::string_view sv, bool first) noexcept;
    bool doHeadersEnd() noexcept;
    bool doBody(std::string_view sv) noexcept;
    bool doMessageEnd() noexcept;
    bool doChunkHeader(size_t len) noexcept { return true; }
    bool doChunkEnd() noexcept { return true; }

    LogMsg& logMsg() const noexcept
    {
        auto& ref = swirly::logMsg();
        ref << '<' << ep_ << "> ";
        return ref;
    }

  private:
    IoSocket sock_;
    TcpEndpoint ep_;
    RestServ& restServ_;
    FileToken tok_;
    Timer tmr_;
    HttpRequest req_;
    Buffer outbuf_;
    HttpResponse rsp_{outbuf_};
};

} // namespace swirly

#endif // SWIRLYD_HTTPSESS_HPP
