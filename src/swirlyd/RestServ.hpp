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

#include <swirly/sys/Time.hpp>

#include <vector>

namespace swirly {
inline namespace web {
class HttpRequest;
class HttpResponse;
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

    void handleRequest(const HttpRequest& req, HttpResponse& resp) noexcept;

  private:
    bool reset(const HttpRequest& req) noexcept;

    void restRequest(const HttpRequest& req, Time now, HttpResponse& resp);

    void refDataRequest(const HttpRequest& req, Time now, HttpResponse& resp);
    void assetRequest(const HttpRequest& req, Time now, HttpResponse& resp);
    void instrRequest(const HttpRequest& req, Time now, HttpResponse& resp);

    void accntRequest(const HttpRequest& req, Time now, HttpResponse& resp);
    void marketRequest(const HttpRequest& req, Time now, HttpResponse& resp);

    void orderRequest(const HttpRequest& req, Time now, HttpResponse& resp);
    void execRequest(const HttpRequest& req, Time now, HttpResponse& resp);
    void tradeRequest(const HttpRequest& req, Time now, HttpResponse& resp);
    void posnRequest(const HttpRequest& req, Time now, HttpResponse& resp);

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
