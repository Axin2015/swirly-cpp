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
#ifndef DBRPP_MATCH_HPP
#define DBRPP_MATCH_HPP

#include <dbrpp/order.hpp>
#include <dbrpp/posn.hpp>
#include <dbrpp/trade.hpp>

namespace dbrpp {

class Match {
    DbrMatch& impl_;
public:
    explicit
    Match(DbrMatch& impl) noexcept
        : impl_(impl)
    {
    }
    operator DbrMatch&() const noexcept
    {
        return impl_;
    }
    DbrMatch*
    c_arg() const noexcept
    {
        return &impl_;
    }
    bool
    operator ==(Match rhs) const noexcept
    {
        return impl_.id == rhs.impl_.id;
    }
    bool
    operator !=(Match rhs) const noexcept
    {
        return impl_.id != rhs.impl_.id;
    }
    DbrIden
    id() const noexcept
    {
        return impl_.id;
    }
    Order
    maker_order() const noexcept
    {
        return Order(*impl_.maker_order);
    }
    Posn
    maker_posn() const noexcept
    {
        return Posn(*impl_.maker_posn);
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_.ticks;
    }
    DbrLots
    lots() const noexcept
    {
        return impl_.lots;
    }
    Trade
    taker_trade() const noexcept
    {
        return Trade(*impl_.taker_trade);
    }
    Trade
    maker_trade() const noexcept
    {
        return Trade(*impl_.maker_trade);
    }
};

inline std::ostream&
operator <<(std::ostream& os, Match match)
{
    return os << "id=" << match.id()
              << ",maker_order=" << match.maker_order().id()
              << ",maker_posn=" << match.maker_posn().id()
              << ",ticks=" << match.ticks()
              << ",lots=" << match.lots()
              << ",taker_trade=" << match.taker_trade().id()
              << ",maker_trade=" << match.maker_trade().id();
}
} // dbrpp

#endif // DBRPP_MATCH_HPP
