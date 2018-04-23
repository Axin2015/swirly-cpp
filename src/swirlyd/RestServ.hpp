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
#ifndef SWIRLYD_RESTSERV_HPP
#define SWIRLYD_RESTSERV_HPP

#include <swirly/util/BasicTypes.hpp>
#include <swirly/util/Profile.hpp>
#include <swirly/util/Symbol.hpp>
#include <swirly/util/Tokeniser.hpp>

#include <swirly/util/Time.hpp>

#include <vector>

namespace swirly {
inline namespace web {
class HttpRequest;
class HttpStream;
class Rest;
} // namespace web

class RestServ {
  public:
    explicit RestServ(Rest& rest) noexcept
    : rest_(rest)
    , profile_{"profile"sv}
    {
    }
    ~RestServ();

    // Copy.
    RestServ(const RestServ& rhs) = delete;
    RestServ& operator=(const RestServ& rhs) = delete;

    // Move.
    RestServ(RestServ&&) = delete;
    RestServ& operator=(RestServ&&) = delete;

    void handleRequest(const HttpRequest& req, HttpStream& os) noexcept;

  private:
    bool reset(const HttpRequest& req) noexcept;

    void restRequest(const HttpRequest& req, Time now, HttpStream& os);

    void refDataRequest(const HttpRequest& req, Time now, HttpStream& os);
    void assetRequest(const HttpRequest& req, Time now, HttpStream& os);
    void instrRequest(const HttpRequest& req, Time now, HttpStream& os);

    void accntRequest(const HttpRequest& req, Time now, HttpStream& os);
    void marketRequest(const HttpRequest& req, Time now, HttpStream& os);

    void orderRequest(const HttpRequest& req, Time now, HttpStream& os);
    void execRequest(const HttpRequest& req, Time now, HttpStream& os);
    void tradeRequest(const HttpRequest& req, Time now, HttpStream& os);
    void posnRequest(const HttpRequest& req, Time now, HttpStream& os);

    Rest& rest_;
    bool matchMethod_{false};
    bool matchPath_{false};
    Tokeniser path_;
    std::vector<Id64> ids_;
    std::vector<Symbol> symbols_;
    Profile profile_;
};

} // namespace swirly

#endif // SWIRLYD_RESTSERV_HPP
