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
#ifndef SWIRLYD_FIXMAKERBOT_HXX
#define SWIRLYD_FIXMAKERBOT_HXX

#include "FixHandler.hxx"

#include <swirly/fix/Random.hpp>

#include <swirly/sys/Timer.hpp>

namespace swirly {
inline namespace sys {
class Reactor;
} // namespace sys

class FixMakerBot : public FixHandler {
  public:
    FixMakerBot(CyclTime now, Reactor& r, const FixSessId& sess_id, const Config& config)
    : reactor_(r)
    {
    }
    ~FixMakerBot() override;

    // Copy.
    FixMakerBot(const FixMakerBot&) = delete;
    FixMakerBot& operator=(const FixMakerBot&) = delete;

    // Move.
    FixMakerBot(FixMakerBot&&) = delete;
    FixMakerBot& operator=(FixMakerBot&&) = delete;

  protected:
    void do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id) override;
    void do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                      Disconnect disconnect) noexcept override;
    void do_on_message(CyclTime now, FixConn& conn, std::string_view msg, std::size_t body_off,
                       Version ver, const FixHeaderView& hdr) override;
    void do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept override;
    void do_on_timeout(CyclTime now, const FixConn& conn) noexcept override;
    void do_prepare(CyclTime now, const FixSessId& sess_id,
                    const FixHandlerMap& handler_map) override;
    void do_send(CyclTime now, std::string_view msg_type, std::string_view msg) override;

  private:
    void on_market_data(CyclTime now, Timer& tmr);
    void on_status(CyclTime now, Timer& tmr);

    Reactor& reactor_;
    FixConn* conn_{nullptr};
    Timer md_tmr_, stat_tmr_;
    std::random_device rd_;
    RandomBbo bbo_{rd_};
    int count_{0};
};

} // namespace swirly

#endif // SWIRLYD_FIXMAKERBOT_HXX
