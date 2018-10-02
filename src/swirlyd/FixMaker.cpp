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
#include "FixMaker.hxx"

#include <swirly/fix/Conn.hpp>

#include <swirly/util/Log.hpp>

namespace swirly {
using namespace std;

FixMaker::~FixMaker() = default;

void FixMaker::do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id)
{
    SWIRLY_INFO << sess_id << " <FixMaker> on_logon";
    conn_ = &conn;
}

void FixMaker::do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                            bool disconnect) noexcept
{
    if (!disconnect) {
        SWIRLY_INFO << sess_id << " <FixMaker> on_logout";
    } else {
        SWIRLY_WARNING << sess_id << " <FixMaker> on_logout";
    }
    conn_ = nullptr;
}

void FixMaker::do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off,
                             Version ver, const FixHeader& hdr)
{
    SWIRLY_INFO << conn.sess_id() << " <FixMaker> on_message: " << hdr.msg_type.value;
}

void FixMaker::do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept
{
    SWIRLY_ERROR << conn.sess_id() << " <FixMaker> on_error: " << e.what();
}

void FixMaker::do_on_timeout(CyclTime now, const FixConn& conn) noexcept
{
    SWIRLY_WARNING << conn.sess_id() << " <FixMaker> on_timeout";
}

void FixMaker::do_prepare(CyclTime now, const FixSessId& sess_id, const FixHandlerMap& handler_map)
{
    SWIRLY_INFO << sess_id << " <FixMaker> prepare";
}

void FixMaker::do_send(CyclTime now, string_view msg_type, string_view msg)
{
    if (conn_) {
        auto fn = [msg](CyclTime now, ostream& os) { os << msg; };
        conn_->send(now, msg_type, fn);
    }
}

} // namespace swirly
