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
#ifndef SWIRLYD_RESTAPP_HXX
#define SWIRLYD_RESTAPP_HXX

#include "RestImpl.hxx"

#include <swirly/http/Types.hpp>

#include <swirly/sys/IpAddress.hpp>

#include <swirly/util/Tokeniser.hpp>

#include <vector>

namespace swirly {
inline namespace http {
class HttpStream;
} // namespace http

class RestRequest;

class RestApp {
  public:
    using Transport = Tcp;
    using Endpoint = TcpEndpoint;

    explicit RestApp(LobApp& lob_app)
    : impl_{lob_app}
    {
    }
    ~RestApp();

    // Copy.
    RestApp(const RestApp&) = delete;
    RestApp& operator=(const RestApp&) = delete;

    // Move.
    RestApp(RestApp&&) = delete;
    RestApp& operator=(RestApp&&) = delete;

    void on_connect(CyclTime now, const Endpoint& ep);
    void on_disconnect(CyclTime now, const Endpoint& ep) noexcept;
    void on_error(CyclTime now, const Endpoint& ep, const std::exception& e) noexcept;
    void on_message(CyclTime now, const Endpoint& ep, const RestRequest& req,
                    HttpStream& os) noexcept;
    void on_timeout(CyclTime now, const Endpoint& ep) noexcept;

  private:
    NoCache reset(const RestRequest& req) noexcept;

    void rest_request(CyclTime now, const RestRequest& req, HttpStream& os);

    void ref_data_request(CyclTime now, const RestRequest& req, HttpStream& os);
    void asset_request(CyclTime now, const RestRequest& req, HttpStream& os);
    void instr_request(CyclTime now, const RestRequest& req, HttpStream& os);

    void sess_request(CyclTime now, const RestRequest& req, HttpStream& os);
    void market_request(CyclTime now, const RestRequest& req, HttpStream& os);

    void order_request(CyclTime now, const RestRequest& req, HttpStream& os);
    void exec_request(CyclTime now, const RestRequest& req, HttpStream& os);
    void trade_request(CyclTime now, const RestRequest& req, HttpStream& os);
    void posn_request(CyclTime now, const RestRequest& req, HttpStream& os);

    RestImpl impl_;
    bool match_method_{false};
    bool match_path_{false};
    Tokeniser path_;
    std::vector<Id64> ids_;
    std::vector<Symbol> symbols_;
};

} // namespace swirly

#endif // SWIRLYD_RESTAPP_HXX
