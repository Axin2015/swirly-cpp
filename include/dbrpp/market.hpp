/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#ifndef DBRPP_MARKET_HPP
#define DBRPP_MARKET_HPP

#include <dbrpp/side.hpp>

#include <dbr/market.h>

#include <iostream>

namespace dbrpp {

class Market {
    DbrMarket impl_;
public:
    explicit
    Market(DbrMarket impl) noexcept
        : impl_(impl)
    {
    }
    explicit
    operator DbrMarket() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(Market rhs) const noexcept
    {
        return id() == rhs.id();
    }
    bool
    operator !=(Market rhs) const noexcept
    {
        return id() != rhs.id();
    }
    DbrIden
    id() const noexcept
    {
        return dbr_market_id(impl_);
    }
    Side
    bid_side() const noexcept
    {
        return Side(dbr_market_bid_side(impl_));
    }
    Side
    ask_side() const noexcept
    {
        return Side(dbr_market_ask_side(impl_));
    }
};

inline std::ostream&
operator <<(std::ostream& os, Market market)
{
    return os << "id=" << market.id();
}

} // dbrpp

#endif // DBRPP_MARKET_HPP
