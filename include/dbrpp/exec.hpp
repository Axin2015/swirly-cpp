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
#ifndef DBRPP_EXEC_HPP
#define DBRPP_EXEC_HPP

#include <dbrpp/rec.hpp>

namespace dbr {

/**
 * @addtogroup TypesExec
 * @{
 */

class ExecRef {
    DbrExec* impl_;
public:
    explicit
    ExecRef(DbrExec& impl) noexcept
        : impl_{&impl}
    {
    }
    operator DbrExec&() const noexcept
    {
        return *impl_;
    }
    DbrExec*
    c_arg() const noexcept
    {
        return impl_;
    }
    bool
    operator ==(ExecRef rhs) const noexcept
    {
        return impl_->id == rhs.impl_->id;
    }
    bool
    operator !=(ExecRef rhs) const noexcept
    {
        return impl_->id != rhs.impl_->id;
    }
    DbrIden
    id() const noexcept
    {
        return impl_->id;
    }
    DbrIden
    order() const noexcept
    {
        return impl_->order;
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
    state() const noexcept
    {
        return impl_->c.state;
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
    lots() const noexcept
    {
        return impl_->c.lots;
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
    DbrTicks
    last_ticks() const noexcept
    {
        return impl_->c.last_ticks;
    }
    DbrLots
    last_lots() const noexcept
    {
        return impl_->c.last_lots;
    }
    DbrLots
    min_lots() const noexcept
    {
        return impl_->c.min_lots;
    }
    DbrIden
    match() const noexcept
    {
        return impl_->match;
    }
    AccntRecRef
    cpty() const noexcept
    {
        return AccntRecRef{*impl_->cpty.rec};
    }
    int
    role() const noexcept
    {
        return impl_->role;
    }
    DbrMillis
    created() const noexcept
    {
        return impl_->created;
    }
};

inline std::ostream&
operator <<(std::ostream& os, ExecRef exec)
{
    return os << "id=" << exec.id()
              << ",order=" << exec.order()
              << ",trec=" << exec.trec().mnem()
              << ",arec=" << exec.arec().mnem()
              << ",crec=" << exec.crec().mnem()
              << ",settl_date=" << exec.settl_date()
              << ",ref=" << exec.ref()
              << ",state=" << exec.state()
              << ",action=" << exec.action()
              << ",ticks=" << exec.ticks()
              << ",lots=" << exec.lots()
              << ",resd=" << exec.resd()
              << ",exec=" << exec.exec()
              << ",last_ticks=" << exec.last_ticks()
              << ",last_lots=" << exec.last_lots()
              << ",min_lots=" << exec.min_lots()
              << ",match=" << exec.match()
              << ",cpty=" << exec.cpty().mnem()
              << ",role=" << exec.role()
              << ",created=" << exec.created();
}

/** @} */

} // dbr

#endif // DBRPP_EXEC_HPP
