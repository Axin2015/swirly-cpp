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
#ifndef SWIRLY_HTTP_APP_HPP
#define SWIRLY_HTTP_APP_HPP

#include <swirly/http/Conn.hpp>

#include <swirly/util/Time.hpp>
#include <swirly/util/Version.hpp>

namespace swirly {
inline namespace http {

class HttpRequest;
class HttpStream;

class SWIRLY_API HttpApp {
  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    HttpApp() noexcept = default;
    virtual ~HttpApp();

    // Copy.
    constexpr HttpApp(const HttpApp&) noexcept = default;
    HttpApp& operator=(const HttpApp&) noexcept = default;

    // Move.
    constexpr HttpApp(HttpApp&&) noexcept = default;
    HttpApp& operator=(HttpApp&&) noexcept = default;

    void on_connect(CyclTime now, const Endpoint& ep) { do_on_connect(now, ep); }
    void on_disconnect(CyclTime now, const Endpoint& ep) noexcept { do_on_disconnect(now, ep); }
    void on_error(CyclTime now, const Endpoint& ep, const std::exception& e) noexcept
    {
        do_on_error(now, ep, e);
    }
    void on_message(CyclTime now, const Endpoint& ep, const HttpRequest& req, HttpStream& os)
    {
        do_on_message(now, ep, req, os);
    }
    void on_timeout(CyclTime now, const Endpoint& ep) noexcept { do_on_timeout(now, ep); }

  protected:
    virtual void do_on_connect(CyclTime now, const Endpoint& ep) = 0;
    virtual void do_on_disconnect(CyclTime now, const Endpoint& ep) noexcept = 0;
    virtual void do_on_error(CyclTime now, const Endpoint& ep, const std::exception& e) noexcept
        = 0;
    virtual void do_on_message(CyclTime now, const Endpoint& ep, const HttpRequest& req,
                               HttpStream& os)
        = 0;
    virtual void do_on_timeout(CyclTime now, const Endpoint& ep) noexcept = 0;
};

} // namespace http
} // namespace swirly

#endif // SWIRLY_HTTP_APP_HPP
