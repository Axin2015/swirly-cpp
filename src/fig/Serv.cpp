/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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
#include <swirly/fig/Serv.hpp>

#include <swirly/fig/TraderSess.hpp>

#include <swirly/elm/Exception.hpp>

using namespace std;

namespace swirly {

struct Serv::Impl {
    RecSet traders;
};

Serv::Serv()
:   impl_{make_unique<Impl>()}
{
}

Serv::~Serv() noexcept = default;

Serv::Serv(Serv&&) = default;

Serv& Serv::operator =(Serv&&) = default;

const TraderSess& Serv::createTrader(const StringView& mnem, const StringView& display,
                                     const StringView& email)
{
    Rec& rec = impl_->traders.insert(make_unique<TraderSess>(mnem, display, email));
    return static_cast<TraderSess&>(rec);
}

const TraderSess& Serv::updateTrader(const StringView& mnem, const StringView& display)
{
    auto it = impl_->traders.find(mnem);
    if (it == impl_->traders.end())
        throwException<TraderNotFoundException>("trader '%.*s' does not exist", SWIRLY_STR(mnem));
    return static_cast<const TraderSess&>(*it);
}

} // swirly
