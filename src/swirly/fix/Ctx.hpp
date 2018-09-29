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
#ifndef SWIRLY_FIX_CTX_HPP
#define SWIRLY_FIX_CTX_HPP

#include <swirly/fix/Acceptor.hpp>
#include <swirly/fix/Initiator.hpp>

namespace swirly {
inline namespace fix {

class SWIRLY_API FixCtx {
  public:
    FixCtx(CyclTime now, Reactor& r, std::istream& config, FixApp& app);
    FixCtx(CyclTime now, Reactor& r, std::istream&& config, FixApp& app)
    : FixCtx{now, r, config, app}
    {
    }
    FixCtx(CyclTime now, Reactor& r, const char* config, FixApp& app);
    ~FixCtx();

    // Copy.
    FixCtx(const FixCtx&) = delete;
    FixCtx& operator=(const FixCtx&) = delete;

    // Move.
    FixCtx(FixCtx&&) noexcept;
    FixCtx& operator=(FixCtx&&) noexcept;

  private:
    std::unique_ptr<Config> root_config_;
    std::map<TcpEndpoint, FixAcceptor> acceptors_;
    std::list<FixInitiator> initiators_;
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_CTX_HPP
