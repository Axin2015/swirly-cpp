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
#ifndef DBRPP_TRADE_HPP
#define DBRPP_TRADE_HPP

#include <dbrpp/rec.hpp>

namespace dbr {

class Trade {
    DbrTrade& impl_;
public:
    explicit
    Trade(DbrTrade& impl) noexcept
        : impl_(impl)
    {
    }
    operator DbrTrade&() const noexcept
    {
        return impl_;
    }
    DbrTrade*
    c_arg() const noexcept
    {
        return &impl_;
    }
    bool
    operator ==(Trade rhs) const noexcept
    {
        return impl_.id == rhs.impl_.id;
    }
    bool
    operator !=(Trade rhs) const noexcept
    {
        return impl_.id != rhs.impl_.id;
    }
    DbrIden
    id() const noexcept
    {
        return impl_.id;
    }
    DbrIden
    match() const noexcept
    {
        return impl_.match;
    }
    DbrIden
    order() const noexcept
    {
        return impl_.order;
    }
    int
    order_rev() const noexcept
    {
        return impl_.order_rev;
    }
    TraderRec
    trec() const noexcept
    {
        return TraderRec(*impl_.trader.rec);
    }
    AccntRec
    arec() const noexcept
    {
        return AccntRec(*impl_.accnt.rec);
    }
    Ref
    ref() const noexcept
    {
        return Ref(impl_.ref);
    }
    MarketRec
    mrec() const noexcept
    {
        return MarketRec(*impl_.market.rec);
    }
    AccntRec
    cpty() const noexcept
    {
        return AccntRec(*impl_.cpty.rec);
    }
    int
    role() const noexcept
    {
        return impl_.role;
    }
    int
    action() const noexcept
    {
        return impl_.action;
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_.ticks;
    }
    DbrLots
    resd() const noexcept
    {
        return impl_.resd;
    }
    DbrLots
    exec() const noexcept
    {
        return impl_.exec;
    }
    DbrLots
    lots() const noexcept
    {
        return impl_.lots;
    }
    DbrDate
    settl_date() const noexcept
    {
        return impl_.settl_date;
    }
    DbrMillis
    created() const noexcept
    {
        return impl_.created;
    }
    DbrMillis
    modified() const noexcept
    {
        return impl_.modified;
    }
};

inline std::ostream&
operator <<(std::ostream& os, Trade trade)
{
    return os << "id=" << trade.id()
              << ",match=" << trade.match()
              << ",order=" << trade.order()
              << ",order_rev=" << trade.order_rev()
              << ",trec=" << trade.trec().mnem()
              << ",arec=" << trade.arec().mnem()
              << ",ref=" << trade.ref()
              << ",mrec=" << trade.mrec().mnem()
              << ",cpty=" << trade.cpty().mnem()
              << ",role=" << trade.role()
              << ",action=" << trade.action()
              << ",ticks=" << trade.ticks()
              << ",resd=" << trade.resd()
              << ",exec=" << trade.exec()
              << ",lots=" << trade.lots()
              << ",settl_date=" << trade.settl_date()
              << ",created=" << trade.created()
              << ",modified=" << trade.modified();
}

inline char*
write_entity(char* buf, Trade trade)
{
    buf = dbr_write_trade(buf, trade.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline const char*
read_entity(const char* buf, Trade trade)
{
    buf = dbr_read_trade(buf, trade.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline int
entity_len(Trade trade)
{
    const int len = dbr_trade_len(trade.c_arg());
    if (len < 0)
        throw_exception();
    return len;
}
} // dbr

#endif // DBRPP_TRADE_HPP
