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
#include "FixApp.hxx"
#include "FixMaker.hxx"
#include "FixMakerBot.hxx"

#include <swirly/fix/Conn.hpp>

#include <swirly/util/Log.hpp>

namespace swirly {
using namespace std;
namespace {
FixHandlerPtr make_handler(CyclTime now, const std::string& name, Reactor& r,
                           const FixSessId& sess_id, const Config& config, LobApp& lob_app)
{
    FixHandlerPtr h;
    if (name == "Maker") {
        h = make_unique<FixMaker>(now, r, sess_id, config, lob_app);
    } else if (name == "MakerBot") {
        h = make_unique<FixMakerBot>(now, r, sess_id, config);
    } else {
        throw runtime_error{"invalid handler: " + name};
    }
    return h;
}
} // namespace

FixApp::~FixApp() = default;

// FixHandler.

void FixApp::do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id)
{
    assert(!sess_id.empty());
    assert(handler_map_.count(sess_id));
    handler_map_[sess_id]->on_logon(now, conn, sess_id);
}

void FixApp::do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                          bool disconnect) noexcept
{
    assert(!sess_id.empty());
    assert(handler_map_.count(sess_id));
    handler_map_[sess_id]->on_logout(now, conn, sess_id, disconnect);
}

void FixApp::do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off,
                           Version ver, const FixHeader& hdr)
{
    const auto& sess_id = conn.sess_id();
    if (!sess_id.empty()) {
        assert(handler_map_.count(sess_id));
        handler_map_[sess_id]->on_message(now, conn, msg, body_off, ver, hdr);
    }
}

void FixApp::do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept
{
    const auto& sess_id = conn.sess_id();
    if (!sess_id.empty()) {
        assert(handler_map_.count(sess_id));
        handler_map_[sess_id]->on_error(now, conn, e);
    } else {
        SWIRLY_ERROR << conn.endpoint() << " on_error: " << e.what();
    }
}

void FixApp::do_on_timeout(CyclTime now, const FixConn& conn) noexcept
{
    const auto& sess_id = conn.sess_id();
    if (!sess_id.empty()) {
        assert(handler_map_.count(sess_id));
        handler_map_[sess_id]->on_timeout(now, conn);
    } else {
        SWIRLY_WARNING << conn.endpoint() << " on_timeout";
    }
}

// FixApp.

void FixApp::do_config(CyclTime now, const FixSessId& sess_id, const Config& config)
{
    SWIRLY_INFO << sess_id << " on_config";
    assert(!sess_id.empty());
    handler_map_[sess_id]
        = make_handler(now, config.get("handler"), reactor_, sess_id, config, lob_app_);
}

void FixApp::do_prepare(CyclTime now)
{
    for (auto& [sess_id, handler] : handler_map_) {
        handler->prepare(now, sess_id, handler_map_);
    }
}

void FixApp::do_on_connect(CyclTime now, FixConn& conn)
{
    SWIRLY_INFO << conn.endpoint() << " on_connect";
}

void FixApp::do_on_disconnect(CyclTime now, const FixConn& conn) noexcept
{
    SWIRLY_INFO << conn.endpoint() << " on_disconnect";
}

} // namespace swirly
