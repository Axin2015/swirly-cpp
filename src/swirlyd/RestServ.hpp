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
#include <swirly/util/Symbol.hpp>
#include <swirly/util/Time.hpp>
#include <swirly/util/Tokeniser.hpp>

#include <vector>

namespace swirly {
inline namespace web {
class HttpRequest;
class HttpStream;
class RestApp;
} // namespace web

class RestServ {
  public:
    explicit RestServ(RestApp& app) noexcept
    : app_(app)
    {
    }
    ~RestServ();

    // Copy.
    RestServ(const RestServ& rhs) = delete;
    RestServ& operator=(const RestServ& rhs) = delete;

    // Move.
    RestServ(RestServ&&) = delete;
    RestServ& operator=(RestServ&&) = delete;

    void handle_request(const HttpRequest& req, HttpStream& os) noexcept;

  private:
    bool reset(const HttpRequest& req) noexcept;

    void rest_request(const HttpRequest& req, Time now, HttpStream& os);

    void ref_data_request(const HttpRequest& req, Time now, HttpStream& os);
    void asset_request(const HttpRequest& req, Time now, HttpStream& os);
    void instr_request(const HttpRequest& req, Time now, HttpStream& os);

    void sess_request(const HttpRequest& req, Time now, HttpStream& os);
    void market_request(const HttpRequest& req, Time now, HttpStream& os);

    void order_request(const HttpRequest& req, Time now, HttpStream& os);
    void exec_request(const HttpRequest& req, Time now, HttpStream& os);
    void trade_request(const HttpRequest& req, Time now, HttpStream& os);
    void posn_request(const HttpRequest& req, Time now, HttpStream& os);

    RestApp& app_;
    bool match_method_{false};
    bool match_path_{false};
    Tokeniser path_;
    std::vector<Id64> ids_;
    std::vector<Symbol> symbols_;
};

} // namespace swirly

#endif // SWIRLYD_RESTSERV_HPP
