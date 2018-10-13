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

#include <swirly/fix/Handler.hpp>
#include <swirly/fix/Header.hpp>

namespace swirly {
inline namespace util {
class Config;
} // namespace util
inline namespace fix {

class FixConn;

class SWIRLY_API FixAppBase : public FixHandlerBase {
  public:
    FixAppBase() noexcept = default;
    ~FixAppBase() override;

    // Copy.
    FixAppBase(const FixAppBase&) noexcept;
    FixAppBase& operator=(const FixAppBase&) noexcept;

    // Move.
    FixAppBase(FixAppBase&&) noexcept;
    FixAppBase& operator=(FixAppBase&&) noexcept;

    void config(CyclTime now, const FixSessId& sess_id, const Config& config)
    {
        do_config(now, sess_id, config);
    }
    void prepare(CyclTime now) { do_prepare(now); }
    void on_connect(CyclTime now, FixConn& conn) { do_on_connect(now, conn); }

    /**
     * Called when a disconnect occurs. The connection is disposed immediately after the callback
     * returns.
     */
    void on_disconnect(CyclTime now, const FixConn& conn) noexcept { do_on_disconnect(now, conn); }

  protected:
    virtual void do_config(CyclTime now, const FixSessId& sess_id, const Config& config) = 0;
    virtual void do_prepare(CyclTime now) = 0;
    virtual void do_on_connect(CyclTime now, FixConn& conn) = 0;
    virtual void do_on_disconnect(CyclTime now, const FixConn& conn) noexcept = 0;
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_APP_HPP
