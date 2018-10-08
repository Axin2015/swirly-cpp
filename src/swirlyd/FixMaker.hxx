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
#ifndef SWIRLYD_FIXMAKER_HXX
#define SWIRLYD_FIXMAKER_HXX

#include "FixHandler.hxx"

#include <swirly/fin/Order.hpp>

#include <swirly/fix/Msg.hpp>

namespace swirly {
inline namespace lob {
class LobApp;
class Sess;
} // namespace lob
inline namespace sys {
class Reactor;
} // namespace sys

class FixMaker : public FixHandler {
    using OrderMap = std::unordered_map<Id64, std::vector<OrderPtr>>;

  public:
    FixMaker(CyclTime now, Reactor& r, const FixSessId& sess_id, const Config& config,
             LobApp& lob_app);
    ~FixMaker() override;

    // Copy.
    FixMaker(const FixMaker&) = delete;
    FixMaker& operator=(const FixMaker&) = delete;

    // Move.
    FixMaker(FixMaker&&) = delete;
    FixMaker& operator=(FixMaker&&) = delete;

  protected:
    void do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id) override;
    void do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                      Disconnect disconnect) noexcept override;
    void do_on_message(CyclTime now, FixConn& conn, std::string_view msg, std::size_t body_off,
                       Version ver, const FixHeader& hdr) override;
    void do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept override;
    void do_on_timeout(CyclTime now, const FixConn& conn) noexcept override;
    void do_prepare(CyclTime now, const FixSessId& sess_id,
                    const FixHandlerMap& handler_map) override;
    void do_send(CyclTime now, std::string_view msg_type, std::string_view msg) override;

  private:
    void on_market_data_snapshot(CyclTime now, FixConn& conn, std::string_view msg,
                                 std::size_t body_off, Version ver, const FixHeader& hdr);
    void on_trade(CyclTime now, const Sess& sess, const ExecPtr& exec);

    LobApp& lob_app_;
    const Sess& sess_;
    FixConn* conn_{nullptr};
    MarketDataSnapshot mds_;
    OrderMap order_map_;
};

} // namespace swirly

#endif // SWIRLYD_FIXMAKER_HXX
