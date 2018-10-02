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

namespace swirly {
inline namespace sys {
class Reactor;
} // namespace sys

class FixMaker : public FixHandler {
  public:
    FixMaker(CyclTime now, Reactor& r, const FixSessId& sess_id, const Config& config) {}
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
                      bool disconnect) noexcept override;
    void do_on_message(CyclTime now, FixConn& conn, std::string_view msg, std::size_t body_off,
                       Version ver, const FixHeader& hdr) override;
    void do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept override;
    void do_on_timeout(CyclTime now, const FixConn& conn) noexcept override;
    void do_prepare(CyclTime now, const FixSessId& sess_id,
                    const FixHandlerMap& handler_map) override;
    void do_send(CyclTime now, std::string_view msg_type, std::string_view msg) override;

  private:
    FixConn* conn_{nullptr};
};

} // namespace swirly

#endif // SWIRLYD_FIXMAKER_HXX
