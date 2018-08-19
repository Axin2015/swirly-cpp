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

#include <swirly/util/Time.hpp>
#include <swirly/util/Version.hpp>

namespace swirly {
inline namespace fix {

struct FixHdr;
class FixSess;

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

    void on_connect(FixSess& sess, Time now) noexcept { do_on_connect(sess, now); }
    void on_disconnect(FixSess& sess, Time now) noexcept { do_on_disconnect(sess, now); }
    void on_logon(FixSess& sess, Time now) noexcept { do_on_logon(sess, now); }
    void on_logout(FixSess& sess, Time now) noexcept { do_on_logout(sess, now); }
    void on_message(FixSess& sess, std::string_view msg, std::size_t body_off, Version ver,
                    const FixHdr& hdr, Time now) noexcept
    {
        do_on_message(sess, msg, body_off, ver, hdr, now);
    }

  protected:
    virtual void do_on_connect(FixSess& sess, Time now) noexcept = 0;
    virtual void do_on_disconnect(FixSess& sess, Time now) noexcept = 0;
    virtual void do_on_logon(FixSess& sess, Time now) noexcept = 0;
    virtual void do_on_logout(FixSess& sess, Time now) noexcept = 0;
    virtual void do_on_message(FixSess& sess, std::string_view msg, std::size_t body_off,
                               Version ver, const FixHdr& hdr, Time now) noexcept
        = 0;
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_APP_HPP
