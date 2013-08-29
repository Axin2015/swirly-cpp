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

namespace dbr {

class MatchRef {
    DbrMatch* impl_;
public:
    explicit
    MatchRef(DbrMatch& impl) noexcept
        : impl_(&impl)
    {
    }
    operator DbrMatch&() const noexcept
    {
        return *impl_;
    }
    DbrMatch*
    c_arg() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(MatchRef rhs) const noexcept
    {
        return impl_->id == rhs.impl_->id;
    }
    bool
    operator !=(MatchRef rhs) const noexcept
    {
        return impl_->id != rhs.impl_->id;
    }
    DbrIden
    id() const noexcept
    {
        return impl_->id;
    }
    OrderRef
    maker_order() const noexcept
    {
        return OrderRef(*impl_->maker_order);
    }
    PosnRef
    maker_posn() const noexcept
    {
        return PosnRef(*impl_->maker_posn);
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_->ticks;
    }
    DbrLots
    lots() const noexcept
    {
        return impl_->lots;
    }
    TradeRef
    taker_trade() const noexcept
    {
        return TradeRef(*impl_->taker_trade);
    }
    TradeRef
    maker_trade() const noexcept
    {
        return TradeRef(*impl_->maker_trade);
    }
};

inline std::ostream&
operator <<(std::ostream& os, MatchRef match)
{
    return os << "id=" << match.id()
              << ",maker_order=" << match.maker_order().id()
              << ",ticks=" << match.ticks()
              << ",lots=" << match.lots()
              << ",taker_trade=" << match.taker_trade().id()
              << ",maker_trade=" << match.maker_trade().id();
}
} // dbr

#endif // DBRPP_MATCH_HPP
