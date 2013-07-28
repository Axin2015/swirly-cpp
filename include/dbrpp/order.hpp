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
#ifndef DBRPP_ORDER_HPP
#define DBRPP_ORDER_HPP

#include <dbrpp/rec.hpp>

namespace dbr {

class Order {
    DbrOrder& impl_;
public:
    explicit
    Order(DbrOrder& impl) noexcept
        : impl_(impl)
    {
    }
    operator DbrOrder&() const noexcept
    {
        return impl_;
    }
    DbrOrder*
    c_arg() const noexcept
    {
        return &impl_;
    }
    bool
    operator ==(Order rhs) const noexcept
    {
        return impl_.id == rhs.impl_.id;
    }
    bool
    operator !=(Order rhs) const noexcept
    {
        return impl_.id != rhs.impl_.id;
    }
    DbrIden
    id() const noexcept
    {
        return impl_.id;
    }
    int
    rev() const noexcept
    {
        return impl_.rev;
    }
    int
    status() const noexcept
    {
        return impl_.status;
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
    DbrLots
    min() const noexcept
    {
        return impl_.min;
    }
    DbrFlags
    flags() const noexcept
    {
        return impl_.flags;
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
    bool
    done() const noexcept
    {
        return impl_.resd == 0;
    }
};

inline std::ostream&
operator <<(std::ostream& os, Order order)
{
    return os << "id=" << order.id()
              << ",rev=" << order.rev()
              << ",status=" << order.status()
              << ",trec=" << order.trec().mnem()
              << ",arec=" << order.arec().mnem()
              << ",ref=" << order.ref()
              << ",mrec=" << order.mrec().mnem()
              << ",action=" << order.action()
              << ",ticks=" << order.ticks()
              << ",resd=" << order.resd()
              << ",exec=" << order.exec()
              << ",lots=" << order.lots()
              << ",min=" << order.min()
              << ",flags=" << order.flags()
              << ",created=" << order.created()
              << ",modified=" << order.modified();
}

inline int
entity_len(Order order)
{
    const int len = dbr_order_len(order.c_arg());
    if (len < 0)
        throw_exception();
    return len;
}

inline char*
write_entity(char* buf, Order order)
{
    buf = dbr_write_order(buf, order.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}

inline const char*
read_entity(const char* buf, Order order)
{
    buf = dbr_read_order(buf, order.c_arg());
    if (!buf)
        throw_exception();
    return buf;
}
} // dbr

#endif // DBRPP_ORDER_HPP
