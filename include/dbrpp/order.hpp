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

class OrderRef {
    DbrOrder* impl_;
public:
    explicit
    OrderRef(DbrOrder& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrOrder&() const noexcept
    {
        return *impl_;
    }
    DbrOrder*
    c_arg() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(OrderRef rhs) const noexcept
    {
        return impl_->c.id == rhs.impl_->c.id;
    }
    bool
    operator !=(OrderRef rhs) const noexcept
    {
        return impl_->c.id != rhs.impl_->c.id;
    }
    DbrIden
    id() const noexcept
    {
        return impl_->c.id;
    }
    int
    rev() const noexcept
    {
        return impl_->c.rev;
    }
    int
    status() const noexcept
    {
        return impl_->c.status;
    }
    TraderRecRef
    trec() const noexcept
    {
        return TraderRecRef{*impl_->c.trader.rec};
    }
    AccntRecRef
    arec() const noexcept
    {
        return AccntRecRef{*impl_->c.accnt.rec};
    }
    ContrRecRef
    crec() const noexcept
    {
        return ContrRecRef{*impl_->c.contr.rec};
    }
    DbrDate
    settl_date() const noexcept
    {
        return impl_->c.settl_date;
    }
    Ref
    ref() const noexcept
    {
        return Ref{impl_->c.ref};
    }
    int
    action() const noexcept
    {
        return impl_->c.action;
    }
    DbrTicks
    ticks() const noexcept
    {
        return impl_->c.ticks;
    }
    DbrLots
    resd() const noexcept
    {
        return impl_->c.resd;
    }
    DbrLots
    exec() const noexcept
    {
        return impl_->c.exec;
    }
    DbrLots
    lots() const noexcept
    {
        return impl_->c.lots;
    }
    DbrLots
    min() const noexcept
    {
        return impl_->min;
    }
    DbrFlags
    flags() const noexcept
    {
        return impl_->flags;
    }
    DbrMillis
    created() const noexcept
    {
        return impl_->created;
    }
    DbrMillis
    modified() const noexcept
    {
        return impl_->modified;
    }
    bool
    done() const noexcept
    {
        return impl_->c.resd == 0;
    }
};

inline std::ostream&
operator <<(std::ostream& os, OrderRef order)
{
    return os << "id=" << order.id()
              << ",rev=" << order.rev()
              << ",status=" << order.status()
              << ",trec=" << order.trec().mnem()
              << ",arec=" << order.arec().mnem()
              << ",crec=" << order.crec().mnem()
              << ",settl_date=" << order.settl_date()
              << ",ref=" << order.ref()
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

inline size_t
order_len(const DbrOrder& order, DbrBool enriched) noexcept
{
    return dbr_order_len(&order, enriched);
}

inline char*
write_order(char* buf, const DbrOrder& order, DbrBool enriched) noexcept
{
    return dbr_write_order(buf, &order, enriched);
}

inline const char*
read_order(const char* buf, DbrOrder& order)
{
    buf = dbr_read_order(buf, &order);
    if (!buf)
        throw_exception();
    return buf;
}
} // dbr

#endif // DBRPP_ORDER_HPP
