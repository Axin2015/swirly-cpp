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
#ifndef SWIRLYD_RESTAPP_HPP
#define SWIRLYD_RESTAPP_HPP

#include <swirly/sys/IpAddress.hpp>

#include <swirly/util/IntTypes.hpp>
#include <swirly/util/Symbol.hpp>
#include <swirly/util/Time.hpp>
#include <swirly/util/Tokeniser.hpp>

#include <vector>

namespace swirly {
inline namespace http {
class HttpStream;
} // namespace http

class RestImpl;
class RestRequest;

class RestApp {
  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    explicit RestApp(RestImpl& impl) noexcept
    : impl_(impl)
    {
    }
    ~RestApp();

    // Copy.
    RestApp(const RestApp& rhs) = delete;
    RestApp& operator=(const RestApp& rhs) = delete;

    // Move.
    RestApp(RestApp&&) = delete;
    RestApp& operator=(RestApp&&) = delete;

    void on_connect(Time now, const Endpoint& ep);
    void on_disconnect(Time now, const Endpoint& ep) noexcept;
    void on_error(Time now, const Endpoint& ep, const std::exception& e) noexcept;
    void on_message(Time now, const Endpoint& ep, const RestRequest& req, HttpStream& os) noexcept;
    void on_timeout(Time now, const Endpoint& ep) noexcept;

  private:
    bool reset(const RestRequest& req) noexcept;

    void rest_request(Time now, const RestRequest& req, HttpStream& os);

    void ref_data_request(Time now, const RestRequest& req, HttpStream& os);
    void asset_request(Time now, const RestRequest& req, HttpStream& os);
    void instr_request(Time now, const RestRequest& req, HttpStream& os);

    void sess_request(Time now, const RestRequest& req, HttpStream& os);
    void market_request(Time now, const RestRequest& req, HttpStream& os);

    void order_request(Time now, const RestRequest& req, HttpStream& os);
    void exec_request(Time now, const RestRequest& req, HttpStream& os);
    void trade_request(Time now, const RestRequest& req, HttpStream& os);
    void posn_request(Time now, const RestRequest& req, HttpStream& os);

    RestImpl& impl_;
    bool match_method_{false};
    bool match_path_{false};
    Tokeniser path_;
    std::vector<Id64> ids_;
    std::vector<Symbol> symbols_;
};

} // namespace swirly

#endif // SWIRLYD_RESTAPP_HPP
