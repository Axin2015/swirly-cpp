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
#ifndef SWIRLY_FIX_APP_HPP
#define SWIRLY_FIX_APP_HPP

#include <swirly/fix/SessId.hpp>

#include <swirly/util/Time.hpp>

namespace swirly {
inline namespace fix {

class FixConn;
struct FixHeader;

class SWIRLY_API FixApp {
  public:
    FixApp() noexcept = default;
    virtual ~FixApp();

    // Copy.
    constexpr FixApp(const FixApp&) noexcept = default;
    FixApp& operator=(const FixApp&) noexcept = default;

    // Move.
    constexpr FixApp(FixApp&&) noexcept = default;
    FixApp& operator=(FixApp&&) noexcept = default;

    void on_connect(WallTime now, FixConn& conn) { do_on_connect(now, conn); }
    void on_logon(WallTime now, FixConn& conn, const FixSessId& sess_id)
    {
        do_on_logon(now, conn, sess_id);
    }
    void on_logout(WallTime now, FixConn& conn, const FixSessId& sess_id) noexcept
    {
        do_on_logout(now, conn, sess_id);
    }
    void on_message(WallTime now, FixConn& conn, std::string_view msg, std::size_t body_off,
                    Version ver, const FixHeader& hdr)
    {
        do_on_message(now, conn, msg, body_off, ver, hdr);
    }
    // Terminal conditions follow.
    /**
     * Called on passive disconnect by peer. The connection is disposed immediately after the
     * callback returns.
     */
    void on_disconnect(WallTime now, const FixConn& conn) noexcept { do_on_disconnect(now, conn); }
    /**
     * Called when an error occurs. The connection is disposed immediately after the callback
     * returns.
     */
    void on_error(WallTime now, const FixConn& conn, const std::exception& e) noexcept
    {
        do_on_error(now, conn, e);
    }
    /**
     * Called when no data is received from peer for timeout period. The connection is disposed
     * immediately after the callback returns.
     */
    void on_timeout(WallTime now, const FixConn& conn) noexcept { do_on_timeout(now, conn); }

  protected:
    virtual void do_on_connect(WallTime now, FixConn& conn) = 0;
    virtual void do_on_logon(WallTime now, FixConn& conn, const FixSessId& sess_id) = 0;
    virtual void do_on_logout(WallTime now, FixConn& conn, const FixSessId& sess_id) noexcept = 0;
    virtual void do_on_message(WallTime now, FixConn& conn, std::string_view msg,
                               std::size_t body_off, Version ver, const FixHeader& hdr)
        = 0;

    virtual void do_on_disconnect(WallTime now, const FixConn& conn) noexcept = 0;
    virtual void do_on_error(WallTime now, const FixConn& conn, const std::exception& e) noexcept
        = 0;
    virtual void do_on_timeout(WallTime now, const FixConn& conn) noexcept = 0;
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_APP_HPP
