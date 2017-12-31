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
#ifndef SWIRLYD_HTTPSERV_HPP
#define SWIRLYD_HTTPSERV_HPP

#include <swirly/Config.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/asio.hpp>
#pragma GCC diagnostic pop

namespace swirly {

class RestServ;

class HttpServ {
  public:
    HttpServ(boost::asio::io_service& ioServ, std::uint16_t port, RestServ& restServ);
    ~HttpServ() noexcept;

    // Copy.
    HttpServ(const HttpServ&) = delete;
    HttpServ& operator=(const HttpServ&) = delete;

    // Move.
    HttpServ(HttpServ&&) = delete;
    HttpServ& operator=(HttpServ&&) = delete;

  private:
    void asyncAccept();

    boost::asio::io_service& ioServ_;
    boost::asio::ip::tcp::acceptor acceptor_;
    RestServ& restServ_;
};

} // namespace swirly

#endif // SWIRLYD_HTTPSERV_HPP
