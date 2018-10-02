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
#include "Ctx.hpp"

#include "App.hpp"

#include <fstream>

namespace swirly {
inline namespace fix {
using namespace std;
namespace {
ifstream open_config(const char* path)
{
    ifstream is{path};
    if (!is.is_open()) {
        throw Exception{make_error_code(errc::io_error), err_msg() << "open failed: " << path};
    }
    return is;
}
} // namespace

FixCtx::FixCtx(CyclTime now, Reactor& r, std::istream& config, FixAppBase& app)
{
    FixConfig fix_config;
    fix_config.read(config);

    FixSessMap& sess_map = fix_config.sess_map;
    FixSessMap::iterator it = sess_map.begin();
    while (it != sess_map.end()) {

        auto node = sess_map.extract(it++);
        const auto& config = node.mapped();
        app.config(now, node.key(), config);

        const std::string& type = config.get("type");
        const auto ep = config.get<TcpEndpoint>("endpoint");
        if (type == "acceptor") {
            auto ep = config.get<TcpEndpoint>("endpoint");
            auto it = acceptors_.lower_bound(ep);
            const bool found{it != acceptors_.end() && !acceptors_.key_comp()(ep, it->first)};
            if (!found) {
                it = acceptors_.try_emplace(it, move(ep), now, r, ep, app);
            }
            it->second.insert(move(node));
        } else if (type == "initiator") {
            initiators_.emplace_back(now, r, ep, move(node), app);
        } else {
            throw runtime_error{"invalid session type: " + type};
        }
    }
    // Session config references root config.
    root_config_ = move(fix_config.root);
    app.prepare(now);
}

FixCtx::FixCtx(CyclTime now, Reactor& r, const char* config, FixAppBase& app)
: FixCtx{now, r, open_config(config), app}
{
}

FixCtx::~FixCtx() = default;

FixCtx::FixCtx(FixCtx&&) noexcept = default;

FixCtx& FixCtx::operator=(FixCtx&&) noexcept = default;

} // namespace fix
} // namespace swirly
